/*
    a header file for the file system
*/

#ifndef FILE_SYS_H
#define FILE_SYS_H
#include "lib.h"
#include"types.h"

#define FILENAME_LEN 32
#define FOUR_KB 4096
/* struct for data block*/
struct data_block_t
{
    uint8_t data[4096];             // a block is 64 KB
};


/* struct for a index node  */
struct inode_t
{
    int32_t length_of_file;         // the length in bytes of the file corresponding to this inode  
    int32_t data_block_num[1023];   // the array of the index of the corresponding data block of this file
};

/* struct for a directory entry*/
struct dentry_t
{
    int8_t filename[FILENAME_LEN];  // file name
    int32_t filetype;               // type of this file
    int32_t inode_num;              // which inode is referred to this file
    int8_t reserved[24];            // something left

};

/* struct for a boot_block as the head of the file system */
struct boot_block_t
{
    int32_t dir_count;              // how many dirctories are there in this file system
    int32_t inode_count;            // how many inodes in this file system
    int32_t data_count;             // how many data blocks in this file system
    int8_t reserved[52];            // something left
    dentry_t direntries[63];        // directory entries
};

extern int32_t read_dentry_by_name(const uint8_t*fname, dentry_t* dentry);
extern int32_t read_denty_by_index(uint32_t index,dentry_t* dentry);
extern int32_t read_data(uint32_t inode, uint32_t offset, uint8_t*buf, uint32_t length);

extern void file_sys_init(uint32_t file_sys_start);

extern int32_t file_open(const uint8_t* filename);
extern int32_t file_close(int32_t fd);
extern int32_t file_read(int32_t fd, void* buf, int32_t nbytes);
extern int32_t file_write(int32_t fd, const void* buf, int32_t nbytes);

extern int32_t dir_open(const uint8_t* filename);
extern int32_t dir_close(int32_t fd);
extern int32_t dir_read(int32_t fd, void* buf, int32_t nbytes);
extern int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes);



boot_block_t* boot_block;
data_block_t* data_block_addr;
int32_t dir_index;
dentry_t glob_dentry; // a global variable used for CP2 due to the absence of file array and file descriptor 
dentry_t glob_dentry_for_dirread;


#endif

