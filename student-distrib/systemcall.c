#include "systemcall.h"
#include "lib.h"
#include "i8259.h"
#include "file_sys.h"
#include "page.h"

#define MAX_PROC    6
#define SIZE_OF_4MB    kernel
#define PROGRAM_IMAGE   0x8048000
#define VIRTUAL_START   32
int32_t process_table[MAX_PROC] = {0,0,0,0,0,0};

int32_t execute(const uint8_t* command) {
    // 1. parse arguments
    uint8_t fname[FNAME_SIZE];
    uint8_t buf[BUFSIZE];
    dentry_t dentry;
    uint32_t offset = 0;
    uint8_t arg[ARG_NUM];
    int i = 0;
    int j = 0;
    int space_counter = 0;
    int start_valid = 0;
    int cmd_length = 0;
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
    start_valid = i;
    while(command[i] != '\0' || command[i] != ' ' || command[i] != '\n'){
        if ((i > 127) || (j > 31 )) return -1;// over load the keyboard size or over load the fname size
        fname[j] = command[i];
        j++;
        cmd_length++;
        i++;
    }
    fname[j] = '\0'; // end with 0

    // 2. executable check
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
    // 6. context switch
}
