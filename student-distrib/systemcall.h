#ifndef _SYSTEMCALL_H
#define _SYSTEMCALL_H

#include "types.h"

#define ARG_NUM 100
#define FNAME_SIZE 32
#define BUFSIZE 4
#define FOUR_BYTE 4

#define MAGIC_ONE       0x7F            // the four magic numbers
#define MAGIC_TWO       0x45
#define MAGIC_THREE     0x4C
#define MAGIC_FOUR      0x46

#define MAX_FILE_DESCRIPTOR 8
#define NUM_
int32_t execute(const uint8_t* command);


struct pcb_t
{
    uint8_t args[ARG_NUM];
    fd_t file_array[MAX_FILE_DESCRIPTOR];
    int8_t pid;
    int8_t parent_pid;
    uint32_t parent_esp;
    uint32_t parent_ebp;
    uint32_t saved_esp;
    uint32_t saved_ebp;

};

struct fd_t
{
    int32_t (*fop_jump_table[4])();
    int32_t inode;
    int32_t file_type;
    int32_t file_position;
    uint32_t flags;
};



#endif

