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
#define HALT_EXCEPTION  0x1F
#define KERNEL_STACK_OF 4
#define MAX_BUFFER_UPPER 127
#define SIZE_OF_NAME 31
#define ENTRY_OFFSET 24

/*
 *  fop_init()
 *  DESCRIPTION: initialize the file operation table of 5 kinds
 *  INPUTS:  NONE
 *  OUTPUTS: NONE
 *  SIDEEFFECT: NONE
 *  RETURN VALUE: NONE
 */
void fop_init(){
    rtc_op.close = RTC_close;
    rtc_op.open = RTC_open;
    rtc_op.read = RTC_read;
    rtc_op.write = RTC_write;

    stdin_op.close = bad_call_close;
    stdin_op.open = bad_call_open;
    stdin_op.read = terminal_read;
    stdin_op.write = terminal_write;

    stdout_op.close = bad_call_close;
    stdout_op.open = bad_call_open;
    stdout_op.read = terminal_read;
    stdout_op.write = terminal_write;

    regular_op.read = file_read;
    regular_op.write = file_write;
    regular_op.open = file_open;
    regular_op.close = file_close;

    dir_op.read = dir_read;
    dir_op.write = dir_write;
    dir_op.open = dir_open;
    dir_op.close = dir_close;
}

int32_t process_table[MAX_PROC] = {0,0,0,0,0,0};
int32_t current_pid = -1;




/*
 *  execute
 *  DESCRIPTION: execute a file
 *  INPUTS:            command: the command to read                
 *  OUTPUTS:            none

 *  SIDEEFFECT: NONE
 *  RETURN VALUE:       0 normal success 
 *                      -1 wrong
 */

int32_t execute(const uint8_t* command) {
    //puts("executes called \n");
    // 1. parse arguments
    uint8_t fname[FNAME_SIZE];
    uint8_t buf[BUFSIZE];
    dentry_t dentry;
    //uint32_t offset = 0;
    uint8_t arg[ARG_NUM];
    uint8_t exe_check[FOUR_BYTE] = {
        MAGIC_ONE, MAGIC_TWO, MAGIC_THREE, MAGIC_FOUR
    };
    uint32_t code_eip = 0;
    pcb_t * parent_pcb;
    int i = 0; // The index for command
    int j = 0; // The index for fname
    int z = 0; // The index for argument
    //int space_counter = 0;
    //int start_valid = 0;
    //int cmd_length = 0;
    int esp, ebp;
    //****************************************************************
    //variables
    int index;
    PDE entry;
    //***************************************************************
    for(j = 0; j < FNAME_SIZE; j++){
        fname[j] = '\0';
    }
    j = 0;

    for(z = 0; z < ARG_NUM; z++){
        arg[z] = '\0';
    }
    z = 0;

    cli();
    if(!command){  // check whether it is null
        return -1;
    }
    while(command[i] == ' '){
        i++;
    }
    //start_valid = i;
    // first set the fname buffer
    while((command[i] != ' ') && (command[i] != '\0')){
        if ((i > MAX_BUFFER_UPPER) || (j > SIZE_OF_NAME )) return -1;// over load the keyboard size or over load the fname size
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
    if(read_dentry_by_name((uint8_t*)fname, &dentry) == -1) return -1;

    // check if the four magic numbers are correct
    if(read_data(dentry.inode_num, 0, buf, FOUR_BYTE) == -1) return -1;
    //if(strncmp((const int8_t*)buf, "ELF", BUFSIZE)) return -1;
    
    for (i = 0; i < FOUR_BYTE; i++) {
        if (buf[i] != exe_check[i])
            return -1;
    }
    
    // get the entry point of the executable
    read_data(dentry.inode_num, ENTRY_OFFSET, buf, FOUR_BYTE);
    code_eip = *((uint32_t*)buf);
    
    // 3. set up program paging
    for (index = 0; index <= MAX_PROC; index ++){                           //judge current process number
        if (index == MAX_PROC){
            puts("The processes reach maximum!\n");
            return -2;
        }
        if (!process_table[index]){
            process_table[index] = 1;
            break;
        }
    }
    current_pid = index;                                                //update current pid

    entry.MBPDE.value = 0;                                      //create page for process
    entry.MBPDE.present = 1;
    entry.MBPDE.R_W = 1;
    entry.MBPDE.page_size = 1;
    entry.MBPDE.user_or_Supervisor = 1;
    entry.MBPDE.table_base_add = (uint32_t)((index * SIZE_OF_4MB + 2 * SIZE_OF_4MB)>>22);   //where kernel is a constant which equal to 4 MB, 22 = 32 - 10, becaues page bass in entry is high 10 bit
    PDE_TABLE[VIRTUAL_START] = entry;      //start from 128 MB
    asm volatile(
        "movl %%cr3, %%eax;"
        "movl %%eax, %%cr3;"
        ::: "eax"                                           //flush the TLB
    );
    // 4. user-level program loader
    read_data(dentry.inode_num, 0, (uint8_t*)(PROGRAM_IMAGE), SIZE_OF_4MB);             //instore the loader
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
    //strcpy((int8_t*)new_pcb->args,(int8_t*)arg);
    for(z = 0; z < ARG_NUM; z++){
        new_pcb->args[z] = arg[z];
    }
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
    sti();
    asm volatile(
    	"cli;"
    	// User DS
    	"mov $0x2B, %%ax;"
    	"mov %%ax, %%ds;"
    	"pushl $0x2B;"
    	"pushl %1;"  // PUSH ESP
    	
    	"pushfl;" // PUSH EFLAG
		"popl %%edx;"
		"orl $0x200,%%edx;"
		"pushl %%edx;"
    	"pushl $0x23;" // PUSH CS
    	"pushl %0;" //eip
    	"iret;"
    	"BACK_TO_RET:;"
    	"leave;"
    	"ret;"
    	: // no outputs
    	:"r"(code_eip),"r"(SIZE_OF_128MB + SIZE_OF_4MB - WORD_SIZE)// input
    	:"%edx","%eax" 
    );
    return 0;
}
/*
 *  getargs
 *  DESCRIPTION: get argument from the pcb args
 *  INPUTS:             buf to fill the args; nbytes: number of bytes to read                
 *  OUTPUTS:            none

 *  SIDEEFFECT: NONE
 *  RETURN VALUE:       0 normal success
 *                      -1 wrong
 */

int32_t getargs(uint8_t* buf, int32_t nbytes){
    int i = 0;
    int esp;
    // fetch the address of the current PCB
    asm("movl %%esp, %0" : "=r"(esp) :);
    pcb_t *pcb = (pcb_t *)( esp & PCB_MSK);
    if(buf == NULL){
        return -1;
    }
    if((nbytes == 0)){
        return -1;
    }
    if(pcb->args[0] == '\0'){
        return -1;
    }

    for(i = 0; i < nbytes; i++){
        buf[i] = pcb->args[i];
    }
    return 0;
}



/*
 *  halt
 *  DESCRIPTION: Halt a process. If halt is called by exception, return 256, if halt shell, shell restart.
 *  INPUTS:             status  - judge who call halt function                 
 *  OUTPUTS:            none

 *  SIDEEFFECT: NONE
 *  RETURN VALUE:       0 normal success halt
 *                      256 halt by exception
 *                      -1 halt wrong
 */

int32_t halt(uint8_t status){
    //puts("halt called \n");
    int32_t esp;
    int32_t ebp;
    pcb_t* current_pcb_address;
    PDE entry;
    int32_t return_value;
    fd_t* current_filearray;
    int32_t file_index;
    cli();
    if (status == HALT_EXCEPTION){
        return_value = 256;                         //if halt by exception, halt handler should return 256
    }
    else return_value = 0;                          //normal return

    current_pcb_address = (pcb_t*)(SIZE_OF_8MB - SIZE_OF_8KB * (current_pid + 1));      //find the pcb stack in the kernel page
    current_filearray = current_pcb_address->file_array;                                //find the file array
    file_index = 0;
    while (current_filearray[file_index].flags != 0 )
    {
        current_filearray[file_index].file_position = 0;
        current_filearray[file_index].inode = -1;
        current_filearray[file_index].flags = 0;
        current_filearray[file_index].optable_ptr = 0;                                  // close all files
        file_index++;
    }
    esp = current_pcb_address->parent_esp;
    ebp = current_pcb_address->parent_ebp;                                              // restore the parent procss information
    tss.ss0 = KERNEL_DS;
    tss.esp0 = SIZE_OF_8MB - SIZE_OF_8KB * current_pcb_address->parent_pid - KERNEL_STACK_OF;   //restore the tss
    
    entry.MBPDE.value = 0;                                                                  //reload the parent page
    entry.MBPDE.present = 1;
    entry.MBPDE.R_W = 1;
    entry.MBPDE.page_size = 1;
    entry.MBPDE.user_or_Supervisor = 1;
    entry.MBPDE.table_base_add = (uint32_t)((current_pcb_address->parent_pid * SIZE_OF_4MB + 2 * SIZE_OF_4MB) >>22);   //where kernel is a constant which equal to 4 MB
    PDE_TABLE[VIRTUAL_START] = entry;      //start from 128 MB                              //22 = 32 - 10, becaues page bass in entry is high 10 bit
    asm volatile(
        "movl %%cr3, %%eax;"                                                                            //flush the TLB
        "movl %%eax, %%cr3;"
        ::: "eax"
    );
    process_table[current_pid] = 0;
    if (current_pid == 0){
        execute((uint8_t*)"shell");                                                 //judge if shell is halt, if so, restart it
    }
    else{
        current_pid = current_pcb_address->parent_pid;                              //update the current pid
    }
    asm volatile(
        "movl %0, %%eax;"
        "movl %1, %%esp;"
        "movl %2, %%ebp;"
        "jmp BACK_TO_RET"                                                           //store the parameter and resturn to the excute function
        :
        :"g"(return_value), "g" (esp), "g" (ebp)
        :"eax"
    );

    return -1;                                                                      //should never reach here, so return -1 for wrong return
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
    //puts("open called \n");
    int32_t fd,cur_file_type,result;
    dentry_t cur_dentry;
    int esp;
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
    result = pcb->file_array[fd].optable_ptr->open(filename);
    //call the open function
    return fd;
}


/*
 *  close
 *  DESCRIPTION: do the close operation for any kind of the file except for stdin and stdout, 
 *               and makes it available for return from later call to open
 *  INPUTS:             fd  - the fd index of the file to close                  
 *  OUTPUTS: NONE
 *  SIDEEFFECT: NONE
 *  RETURN VALUE:       0 - close successfully
 *                      -1 - the fd is out of rage or equal to 0 or 1 or close a not using file
 */
int32_t close(int32_t fd){
    //puts("close called \n");
    int result;
    int esp;
    // should not close the stdin and stdout, or the fd is out of range
    if(fd == 0 || fd == 1 || fd >=8 || fd <0) return -1;
    // fetch the address of the current PCB
    asm("movl %%esp, %0" : "=r"(esp) :);
    pcb_t *pcb = (pcb_t *)( esp & PCB_MSK);
    // if the file is not in use, close fails
    if(pcb->file_array[fd].flags == 0) return -1;
    // try to close the file
    result = pcb->file_array[fd].optable_ptr->close(fd);
    if(result == 0){
        pcb->file_array[fd].flags = 0;
        pcb->file_array[fd].optable_ptr = NULL;
        pcb->file_array[fd].file_position = 0;
        return 0;
    }
    // if close fails, return -1
    return -1;
}

/* introduction: write is funciton used to do the systemcall_write
 * input: int32_t fd, const void* buf, int32_t nbytes
 * output: the return value of the correspoding write function
 */
int32_t write(int32_t fd, const void* buf, int32_t nbytes)
{
    //puts("write called \n");
    int esp;
    asm("movl %%esp, %0" : "=r"(esp) :);        //get esp from the stack
    pcb_t* pcb = (pcb_t*)(esp & 0x7FE000);      //0x7FE000 is the PCB_mask
    if ((fd<0) || (fd>7) || (buf==NULL) || (nbytes<0))
    {
        return -1;      //check if the input value is valid or not
    }
    if (pcb->file_array[fd].flags == 0)     //check the flag of the pcb
    {
        return -1;
    }
    return pcb->file_array[fd].optable_ptr->write(fd,buf,nbytes);   //call the correspoding write function 
}


/* introduction: read is funciton used to do the systemcall_read
 * input: int32_t fd, const void* buf, int32_t nbytes
 * output: the return value of the correspoding read function
 */
int32_t read(int32_t fd, void* buf, int32_t nbytes)
{
    int esp;
    asm("movl %%esp, %0" : "=r"(esp) :);    //get esp from the stack
    pcb_t* pcb = (pcb_t*)(esp & 0x7FE000);  //0x7FE000 is the PCB_mask
    if ((fd<0) || (fd>7) || (buf==NULL) || (nbytes<0))
    {
        return -1;          //check if the input value is valid or not
    }
    if (pcb->file_array[fd].flags == 0)
    {
        return -1;          //check the flag of the pcb
    }
    //puts("successfully called all");
    return pcb->file_array[fd].optable_ptr->read(fd,buf,nbytes);        //call the correspoding read function 
}

