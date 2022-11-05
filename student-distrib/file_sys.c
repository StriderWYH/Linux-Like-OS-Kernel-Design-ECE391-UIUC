#include "file_sys.h"
#include "systemcall.h"
#include "page.h"


/*
 *  read_dentry_by_name
 *  DESCRIPTION: fetch a dentry with the given file name , can fetch the name of all the three file types(regular file and directory file and RTC file)
 *  INPUTS:             fname  - the name of the file, which is assumed to be '\0' terminated 
 *                      denrty - put the message into this temporary dentry as return, can not ensure the filename is null terminated  
 *  OUTPUTS: NONE
 *  SIDEEFFECT: NONE
 *  RETURN VALUE:       0  - find the dentry with the file name fname successfully
 *                      -1 - the fname is too long or null or no such file called fname
 */
int32_t read_dentry_by_name(const uint8_t*fname, dentry_t* dentry){
    int32_t fname_length;
    int8_t fname_true[33];                     // used for the filename which is not null terminated
    int32_t  nul_flag = 0;                      // flag set to 1 if the current filename is not nul terminated
    int32_t i;
    //int32_t result;                            // store the compare result
    
    fname_length = strlen((int8_t*)fname);
    /* if the fname is null or too long , return -1 */
    if(fname_length <=0 || fname_length > FILENAME_LEN) return -1;
   
    // go through each boot_block's dentry, looking for the dentry with the name fname
    for(i = 0; i < boot_block->dir_count; i++){
        // if the fname in the dentry is not null terminated which means it has exact 32 bytes for the name, no place for '\0'
        if(strlen(boot_block->direntries[i].filename) >= FILENAME_LEN){
            memcpy(fname_true,boot_block->direntries[i].filename,FILENAME_LEN);
            fname_true[32] = '\0'; // set the last char to null terminated
            nul_flag = 1;
        }
        // if the fname in this dentry is 32 B, compare the fname_true with the fname
        if (nul_flag){
            if(!strncmp((int8_t*)fname,(const int8_t*)fname_true,FILENAME_LEN)){
                strncpy(dentry->filename,(int8_t*)fname_true,FILENAME_LEN);
                dentry->filetype = boot_block->direntries[i].filetype;
                dentry->inode_num = boot_block->direntries[i].inode_num;
                return 0;
            }else{
                nul_flag = 0;
                continue;   // if not compatible, check next dentry's fname
            }
        }else{  // otherwise just compare the fname in the dentry with the fname
            if (!strncmp((int8_t*)fname,boot_block->direntries[i].filename,fname_length)){
                strcpy(dentry->filename,boot_block->direntries[i].filename);
                dentry->filetype = boot_block->direntries[i].filetype;
                dentry->inode_num = boot_block->direntries[i].inode_num;
                return 0;
            }else{
                continue;   // if not compatible, check next dentry's fname
            }
        }
        nul_flag = 0;
    }
    return -1;
}
/*
 *  read_dentry_by_index
 *  DESCRIPTION: read the dentry with index index(ar1) to the dentry(arg 2)
 *  INPUTS:             index - the index of the target dentry
 *                      dentry - read the target dentry info, we can not make sure that the fname in dentry is nul terminated
 *  OUTPUTS: NONE
 *  SIDEEFFECT: NONE
 *  RETURN VALUE:  -1 - failure to read dentry
 *                  0 - seccess
 */   
int32_t read_denty_by_index(uint32_t index,dentry_t* dentry){
    uint8_t fname_true[33];  //used for the filename which is not null terminated
    int32_t  nul_flag = 0;                      // flag set to 1 if the current filename is not nul terminated
    if(index < 0 || index >= boot_block->dir_count){
        return -1;
    }
    // if the filename is >= 32, then the filename would not be null terminated, therefore need to use fname_true
    if(strlen(boot_block->direntries[index].filename) >= FILENAME_LEN){
            memcpy(fname_true,boot_block->direntries[index].filename,FILENAME_LEN);
            fname_true[32] = '\0';
            nul_flag = 1;  // set nul_flag to 1, indicating that the fname in the dentry is too long to have null terminated
    }
    if (nul_flag == 1){
        strncpy(dentry->filename,(const int8_t*)fname_true,FILENAME_LEN); // in this case the fname in the dentry couldn't be null terminated
        dentry->filetype = boot_block->direntries[index].filetype;
        dentry->inode_num = boot_block->direntries[index].inode_num;
        return 0;
    }else{
        strcpy(dentry->filename,boot_block->direntries[index].filename);
        dentry->filetype = boot_block->direntries[index].filetype;
        dentry->inode_num = boot_block->direntries[index].inode_num;
        return 0;
    }
}
/*
 *  read_data
 *  DESCRIPTION: read length of bytes start from offset position in the file pointed by the inode to the buf
 *  INPUTS:             inode - indicate the data blocks of the file
 *                      offset - the offset position of the first byte to read
 *                      buf   - read the bytes into the buf
 *                      length - specify the number of bytes to read
 *  OUTPUTS: NONE
 *  SIDEEFFECT: NONE
 *  RETURN VALUE:  -1 - failure to read data
 *                  0 - reached the end of the file
 *                  i - the number of bits have read into the buf
 */                 
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t*buf, uint32_t length){
    
    int32_t i,data_block_count;

    data_block_count = boot_block->data_count;                  // obtain the maximum data block in this filesystem 
    // first check if the inode is valid, if not return -1
    if(inode < 0 || inode >= boot_block->inode_count){
        return -1;
    }
    inode_t* current_inode_addr = (inode_t*)boot_block+inode+1; // obtain the current inode address, the file system's blocks are linear in address
    int32_t data_id_in_block = offset % FOUR_KB;  // initialized as the index of the start data byte in the data block
    int32_t data_block_id = offset / FOUR_KB;     // the index of the start  data block 
    // obtain the start data block
    data_block_t* read_data_block = (data_block_t*)data_block_addr + current_inode_addr->data_block_num[data_block_id];
    // then check if the offset is beyond the scope of the file
    
    if(offset >= current_inode_addr->length_of_file){
        return 0;
    }
   
    for(i = 0; i < length; i++){
        // first check if the current byte is in the range of the file
        if (i + offset >= current_inode_addr->length_of_file ){
             return i; // it reached the end of the file, return the bytes have read now
        }
        buf[i] = read_data_block->data[data_id_in_block];
        data_id_in_block++;
        if(data_id_in_block >= FOUR_KB){
            data_block_id++;
            // check if the next data block eists, if not return -1 as failure
            if(current_inode_addr->data_block_num[data_block_id] >= data_block_count) return -1;
             // if still in the range of the data block, update the ptr to the current read data block and the byte index
            data_id_in_block = 0;
            read_data_block = (data_block_t*)(data_block_addr + current_inode_addr->data_block_num[data_block_id]);
        }

    }
    return length;
}
/*
 *  file_sys_init
 *  DESCRIPTION: initial all the global variables used in the file system 
 *  INPUTS:             file_sys_start - the start address of the file system
 *  OUTPUTS: NONE
 *  SIDEEFFECT: NONE
 *  RETURN VALUE: NONE
 */
void file_sys_init(uint32_t file_sys_start){
    boot_block = (boot_block_t*) file_sys_start;
    data_block_addr = (data_block_t*)boot_block + boot_block->inode_count+1;
    dir_index = 0;
}
/*
 *  file_open 
 *  DESCRIPTION: open a data file by fname both for regular file (type2, including executables)
 *  INPUTS:             filename  - the name of the file to be opened
 *  OUTPUTS: NONE
 *  SIDEEFFECT: NONE
 *  RETURN VALUE:       0  - open successfully
 *                      -1 - fail openning 
 */
int32_t file_open(const uint8_t* filename){
    //int32_t result;
    //result = read_dentry_by_name(filename,&glob_dentry);
    // if read file fails or the current file type is not regular file, return -1
    //if (result == -1) return -1;
    //if (glob_dentry.filetype != 2) return -1;
    // otherwise return 0 as success
    return 0;
}
/*
 *  file_close
 *  DESCRIPTION: close a data file, undo what we did in file_open 
 *  INPUTS:             fd  - the file descriptor of the file to close
 *  OUTPUTS: NONE
 *  SIDEEFFECT: NONE
 *  RETURN VALUE:       0  - close successfully
 *                      -1 - fail closing 
 */
int32_t file_close(int32_t fd){
    return 0;
}
/*
 *  file_write
 *  DESCRIPTION: write into  a data file (not work since till now is read only)
 *  INPUTS:            fd  - file descriptor
 *                     buf - the buffer for the bytes to be writen into the file
 *                     nbytes - the number of bytes to write into 
 *  OUTPUTS: NONE
 *  SIDEEFFECT: NONE
 *  RETURN VALUE:       
 *                      -1 - fail writing
 *                
 */   
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes){
    return -1;
}
/*
 *  file_read
 *  DESCRIPTION: read a data file till the end of the file
 *  INPUTS:            fd  - file descriptor 
 *                     buf - read the bytes in the file and write them into the buf
 *                     offset - start from which byte of the file
 *                     nbytes - the number of bytes to be read 
 *  OUTPUTS: NONE
 *  SIDEEFFECT: NONE
 *  RETURN VALUE:       0  - reached the end of the file
 *                      -1 - fail reading
 *                 result - number of the bytes read successfully
 */                                         
int32_t file_read(int32_t fd,void* buf, int32_t nbytes){
    int32_t result,offset,inode_n;
    int esp;
    // int32_t length
    // fetch the address of the current PCB
    asm("movl %%esp, %0" : "=r"(esp) :);
    pcb_t *pcb = (pcb_t *)( esp & PCB_MSK);

    offset = pcb->file_array[fd].file_position;
    inode_n = pcb->file_array[fd].inode;

    //if not in use, read fails
    if(pcb->file_array[fd].flags == 0) return -1;

    // check if the type of the file is regular file, only which could be read
    //if(glob_dentry.filetype != 2) return -1;
    //length = ((inode_t*)(boot_block + 1 + glob_dentry.inode_num))->length_of_file;

    result = read_data(inode_n,offset,buf,nbytes);
    // if read fails, return -1
    if(result == -1) return -1;
    // update the offset
    pcb->file_array[fd].file_position += result;

    return result;

}
/*
 *  dir_open 
 *  DESCRIPTION: open a  file by filename both for directory or regular file
 *  INPUTS:             filename  - the name of the dir to be opened
 *  OUTPUTS: NONE
 *  SIDEEFFECT: NONE
 *  RETURN VALUE:       0  - open successfully
 *                      -1 - fail openning 
 */
int32_t dir_open(const uint8_t* filename){
    //int32_t result;
    //result = read_dentry_by_name(filename,&glob_dentry_for_dirread);
    // if read fname fails  return -1
    //if (result == -1) return -1;
    // otherwise return 0 as success
    dir_index =0;
    return 0;
 }
/*
 *  dir_close
 *  DESCRIPTION: close a file
 *  INPUTS:             fd  - the file descriptor of the file to be closed
 *  OUTPUTS: NONE
 *  SIDEEFFECT: NONE
 *  RETURN VALUE:       0  - close successfully
 *                      -1 - fail closing 
 */
int32_t dir_close(int32_t fd){
    return 0;
}
/*
 *  dir_read
 *  DESCRIPTION: read the successive directory, if reached the last one, keeps returning 0
 *  INPUTS:            fd  - file descriptor
 *                     buf - read the fname and write them into the buf
 *                     offset - start from which file of the directory
 *                     nbytes - the number of bytes to be read 
 *  OUTPUTS: NONE
 *  SIDEEFFECT: NONE
 *  RETURN VALUE:       0  - reach the last directory
 *                      length - the number of bytes read
 *    
 */             
int32_t dir_read(int32_t fd, void* buf, int32_t nbytes){
    int32_t result,length;
    result = read_denty_by_index(dir_index,&glob_dentry_for_dirread);
    
    if(result == -1)  return 0;
    
    length = strlen(glob_dentry_for_dirread.filename);
    if(length > FILENAME_LEN) length = FILENAME_LEN;


    strncpy(buf,glob_dentry_for_dirread.filename,length);
    dir_index++;
    return length;
}
/*
 *  dir_write
 *  DESCRIPTION: write into  a data file (not work since till now is read only)
 *  INPUTS:            fd  - file descriptor
 *                     buf - the buffer for the bytes to be writen into the file
 *                     nbytes - the number of bytes to write into 
 *  OUTPUTS: NONE
 *  SIDEEFFECT: NONE
 *  RETURN VALUE:       
 *                      -1 - fail writing
 *                
 */   
int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes){
    return -1;
}

