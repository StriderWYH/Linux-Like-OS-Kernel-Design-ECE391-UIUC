#include "systemcall.h"
#include "lib.h"
#include "i8259.h"
#include "file_sys.h"
#include "page.h"
#include "rtc.h"

#define MAX_PROC    6
#define SIZE_OF_4MB     kernel
#define SIZE_OF_8MB     0x800000
#define SIZE_OF_8KB     0x2000
#define SIZE_OF_128MB   0x8000000
#define PROGRAM_IMAGE   0x8048000
#define VIRTUAL_START   32
#define KERNEL_MEM_END  0x800000 
#define WORD_SIZE       4

int32_t process_table[MAX_PROC] = {0,0,0,0,0,0};
// int32_t (*file_table[3][4])() = {
// {RTC_read, RTC_write, RTC_open, RTC_close},
// {dir_read, dir_write, dir_open, dir_close},
// {file_read, file_write, file_open, file_close}
// };
int32_t execute(const uint8_t* command) {
    // 1. parse arguments
    uint8_t fname[FNAME_SIZE];
    uint8_t buf[BUFSIZE];
    dentry_t dentry;
    uint32_t offset = 0;
    uint8_t arg[ARG_NUM];
    uint8_t exe_check[FOUR_BYTE] = {
        MAGIC_ONE, MAGIC_TWO, MAGIC_THREE, MAGIC_FOUR
    };
    uint32_t code_eip = 0;
    pcb_t * parent_pcb;
    int i = 0; // The index for command
    int j = 0; // The index for fname
    int z = 0; // The index for argument
    int space_counter = 0;
    int start_valid = 0;
    int cmd_length = 0;
    int esp, ebp;
    //****************************************************************
    //variables
    int index;
    PDE entry;
    //***************************************************************
    if(!command){  // check whether it is null
        return -1;
    }
    while(command[i] == ' '){
        i++;
    }
    //start_valid = i;
    // first set the fname buffer
    while(command[i] != ' '){
        if ((i > 127) || (j > 31 )) return -1;// over load the keyboard size or over load the fname size
        fname[j] = command[i];
        j++;
        //cmd_length++;
        i++;
    }
    fname[j] = '\0'; // end with 0
    // Then set the argument buffer
    i++; //skip the first space
    while(command[i] != '\0'){
        if(z > 31) return -1; // check whether the argument is out of range
        arg[z] = command[i];
        z++;
        i++;
    }
    arg[z] = '\0'; // end with 0


    // 2. executable check
    if(!read_dentry_by_name((uint8_t*)fname, &dentry)) return -1;

    // check if the four magic numbers are correct
    if(!read_data(dentry.inode_num, 0, buf, FOUR_BYTE)) return -1;
    if(strncmp((const int8_t*)buf, "ELF", BUFSIZE)) return -1;
    /*
    for (i = 0; i < FOUR_BYTE; i++) {
        if (buf[i] != exe_check[i])
            return -1;
    }
    */
    // get the entry point of the executable
    read_data(dentry.inode_num, 24, buf, FOUR_BYTE);
    code_eip = *((uint32_t*)buf);
    
    // 3. set up program paging
    for (index = 0; index <= MAX_PROC; index ++){
        if (index == MAX_PROC){
            return -1;
        }
        if (!process_table[index]) break;
    }
    entry.MBPDE.value = 0;
    entry.MBPDE.present = 1;
    entry.MBPDE.R_W = 1;
    entry.MBPDE.page_size = 1;
    entry.MBPDE.user_or_Supervisor = 1;
    entry.MBPDE.table_base_add = (uint32_t)(index * SIZE_OF_4MB + 2 * SIZE_OF_4MB);   //where kernel is a constant which equal to 4 MB
    PDE_TABLE[VIRTUAL_START] = entry;      //start from 128 MB
    asm volatile(
        "movl %%cr3, %%eax;"
        "movl %%eax, %%cr3;"
        ::: "eax"
    );
    // 4. user-level program loader
    read_data(dentry.inode_num, 0, (uint8_t*)(PROGRAM_IMAGE), SIZE_OF_4MB);
    // 5. create pcb
    pcb_t * new_pcb =  (pcb_t*)(SIZE_OF_8MB - (index + 1) * SIZE_OF_8KB);
    if (index == 0){
        parent_pcb = NULL; // there is no process before
        new_pcb->parent_pid = index;
    }
    else{
        parent_pcb = (pcb_t*)(SIZE_OF_8MB - index * SIZE_OF_8KB);    // parent is the last process
        new_pcb->parent_pid = parent_pcb->pid;     // copy the parent pid
    }
    strncpy(new_pcb->args,arg,100);
    new_pcb->pid = index;

    // fill up the stdin and stdout file

    new_pcb->file_array[0].inode = -1;                   // this field shouldn't be used
    new_pcb->file_array[0].file_position = 0;
    new_pcb->file_array[0].flags = 1;
    new_pcb->file_array[0].optable_ptr = &stdin_op;
    // stdout file
    new_pcb->file_array[1].inode = -1;                   // this field shouldn't be used
    new_pcb->file_array[1].file_position = 0;
    new_pcb->file_array[1].flags = 1;
    new_pcb->file_array[1].optable_ptr = &stdout_op;
    //new_pcb->file_array[0].fop_jump_table = 
    for( i = 0; i < 6; i++){
        new_pcb->file_array[i+2].flags = 0;
        new_pcb->file_array[i+2].inode = -1;  
        new_pcb->file_array[i+2].file_position = 0;
    }
    asm("movl %%ebp, %0" : "=r"(ebp) :);
    new_pcb->parent_ebp = ebp;
    asm("movl %%esp, %0" : "=r"(esp) :);
    new_pcb->parent_esp = esp;
    
    // 6. context switch
    tss.ss0 = KERNEL_DS;
    tss.esp0 = KERNEL_MEM_END - (index) * SIZE_OF_8KB - WORD_SIZE;
    asm volatile(
        "xorl %%eax, %%eax;"
        "movw %w0, %%ax;"
        "movw %%ax, %%ds;"
        "pushl %%eax;"          // push the user data segment information
        "pushl %1;"             // push user program esp
        "pushfl;"               // push eflags
        "popl %%eax;"           // manually enable the interrupt
        "orl $0x200, %%eax;"
        "pushl %%eax;"
        "pushl %2;"             // push code segment information
        "pushl %3;"             // push user program eip
        "iret;"
        "return_to_execute:;"
        : /* no outputs */
        : "g" (USER_DS), "g" (SIZE_OF_128MB + SIZE_OF_4MB - WORD_SIZE), "g" (USER_CS), "g" (code_eip)
        : "eax"
    );
    return 0;
}

/*
 *  bad_call_open
 *  DESCRIPTION: used for terminal, do nothing but return -1
 * 
 */
int32_t bad_call_open(const uint8_t* filename){
    return -1;
}
/*
 *  bad_call_close
 *  DESCRIPTION: used for terminal, do nothing but return -1
 * 
 */
int32_t bad_call_close(int32_t fd){
    return -1;
}
/*
 *  open
 *  DESCRIPTION: do the open operation for any kind of the file, and assaign a free fd to it
 *  INPUTS:             fname  - the name of the file, which is assumed to be '\0' terminated                  
 *  OUTPUTS: NONE
 *  SIDEEFFECT: NONE
 *  RETURN VALUE:       fd - (0-7)the fd index of the opened file, means open the file with the file name fname successfully
 *                      -1 - the fname is too long or null or no such file called fname or open fails
 */
int32_t open( const uint8_t* filename){
    int32_t fd,cur_file_type,result;
    dentry_t cur_dentry;

    // fetch the address of the current PCB
    asm("movl %%esp, %0" : "=r"(esp) :);
    pcb_t *pcb = (pcb_t *)( esp & PCB_MSK);

    // check if the filename is valid
    if(filename == NULL) return -1;

    // find the free descriptor 
    for(fd = 2; fd< 8;fd++){
        if(pcb->file_array[fd].flags == 0){
            break;
        }
    }
    // check whether the fdarray is full 
    if(fd == 8) return -1;
    
    // stdin and stdout use fixed file descriptor index for each of them
    if(!strncmp((int8_t*)filename, (int8_t*)"stdin",5)){
        pcb->file_array[0].optable_ptr = &stdin_op;
        pcb->file_array[0].flags = 1;
        return 0;
    }
    if(!strncmp((int8_t*)filename, (int8_t*)"stdout",6)){
        pcb->file_array[1].optable_ptr = &stdout_op;
        pcb->file_array[1].flags = 1;
        return 1;
    }

    // if the filename(other than stdin or stdout) doesn't exist, return -1
    result = read_dentry_by_name(filename,&cur_dentry);
    if(result == -1) return -1;
    // if valid, update the type
    cur_file_type =  cur_dentry.filetype;

    // based on the type of the file, assign corresponding element to the current fd.
    switch (cur_file_type)
    {
    case 0:
        /* RTC file */
        pcb->file_array[fd].flags = 1;
        pcb->file_array[fd].optable_ptr = &rtc_op;
        pcb->file_array[fd].file_position = 0;
        break;
    case 1:
        /* dir file*/
        pcb->file_array[fd].flags = 1;
        pcb->file_array[fd].optable_ptr = &dir_op;
        pcb->file_array[fd].file_position = 0;
        break;
     case 2:
        /* regular file*/
        pcb->file_array[fd].flags = 1;
        pcb->file_array[fd].optable_ptr = &regular_op;
        pcb->file_array[fd].file_position = 0;
        pcb->file_array[fd].inode = cur_dentry.inode_num;
        break;
    default:
        return -1;
    }

    return fd;
}
/*
 *  close
 *  DESCRIPTION: do the close operation for any kind of the file except for stdin and stdout, 
 *               and makes it available for return from later call to open
 *  INPUTS:             fname  - the name of the file, which is assumed to be '\0' terminated                  
 *  OUTPUTS: NONE
 *  SIDEEFFECT: NONE
 *  RETURN VALUE:       fd - (0-7)the fd index of the opened file, means open the file with the file name fname successfully
 *                      -1 - the fname is too long or null or no such file called fname or open fails
 */
int32_t close(int32_t fd){
    int result;
    // should not close the stdin and stdout, or the fd is out of range
    if(fd == 0 || fd == 1 || fd >=8 || fd <0) return -1;
    // fetch the address of the current PCB
    asm("movl %%esp, %0" : "=r"(esp) :);
    pcb_t *pcb = (pcb_t *)( esp & PCB_MSK);
    // if the file is not in use, close fails
    if(pcb->file_array[fd].flags == 0) return -1;

    result = pcb->file_array[fd].optable_ptr->close(fd);
    if(result == 0){
        pcb->file_array[fd].flags = 0;
        pcb->file_array[fd].optable_ptr = NULL;
        pcb->file_array[fd].file_position = 0;
    }

    return -1;
}
