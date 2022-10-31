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
    uint8_t exe_check[FOUR_BYTE] = {
        MAGIC_ONE, MAGIC_TWO, MAGIC_THREE, MAGIC_FOUR
    };
    uint32_t code_eip = 0;
    int i = 0; // The index for command
    int j = 0; // The index for fname
    int z = 0; // The index for argument
    int space_counter = 0;
    int start_valid = 0;
    int cmd_length = 0;
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
    for (i = 0; i < FOUR_BYTE; i++) {
        if (buf[i] != exe_check[i])
            return -1;
    }
    // get the entry point of the executable
    read_data(dentry.inode_num, 24, buf, FOUR_BYTE);
    code_eip = *((uint32_t*)buf);
    // 3. set up program paging
    // 4. user-level program loader
    // 5. create pcb
    // 6. context switch
}
