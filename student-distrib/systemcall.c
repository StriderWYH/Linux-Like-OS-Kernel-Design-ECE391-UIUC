#include "systemcall.h"
#include "lib.h"
#include "i8259.h"
#include "file_sys.h"



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
    // 4. user-level program loader
    // 5. create pcb
    // 6. context switch
}
