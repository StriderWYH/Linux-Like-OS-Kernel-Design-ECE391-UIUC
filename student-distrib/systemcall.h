#ifndef _SYSTEMCALL_H
#define _SYSTEMCALL_H

#include "types.h"
#include "keyboard.h"
#include "file_sys.h"
#include "lib.h"
#include "rtc.h"

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
    fop_table* optable_ptr;
    int32_t inode;
    int32_t file_position;
    uint32_t flags;
};

struct fop_table
{
    int32_t (*read)(int32_t fd, void* buf, int32_t nbytes);
    int32_t (*write)(int32_t fd, const void*buf, int32_t nbytes);
    int32_t (*open)(const uint8_t* filename);
    int32_t (*close)(int32_t fd);
};

fop_table stdin_op = { terminal_read, terminal_write, terminal_open, terminal_close};
fop_table stdout_op = { terminal_read,terminal_write, terminal_open, terminal_close};
fop_table regular_op = { file_read, file_write, file_open, file_close};
fop_table dir_op = {dir_read, dir_write, dir_open, dir_close};
fop_table rtc_op = { RTC_read, RTC_write, RTC_open, RTC_close};


/* function for file operation */

int32_t open( const uint8_t* filename);
int32_t close(int32_t fd);
int32_t write(int32_t fd, const void* buf, int32_t nbytes);
int32_t read(int32_t fd, void* buf, int32_t nbytes);
#endif

