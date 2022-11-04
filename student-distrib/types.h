/* types.h - Defines to use the familiar explicitly-sized types in this
 * OS (uint32_t, int8_t, etc.).  This is necessary because we don't want
 * to include <stdint.h> when building this OS
 * vim:ts=4 noexpandtab
 */

#ifndef _TYPES_H
#define _TYPES_H

#define NULL 0

#ifndef ASM

/* Types defined here just like in <stdint.h> */
typedef int int32_t;
typedef unsigned int uint32_t;

typedef short int16_t;
typedef unsigned short uint16_t;

typedef char int8_t;
typedef unsigned char uint8_t;

//vva declare **************************************************************************************************
typedef struct PDE_4MB PDE_4MB;
typedef struct PDE_4KB PDE_4KB;
typedef struct PTE PTE;
typedef union PDE PDE;
//**************************************************************************************************************

/* why declare here*/
typedef struct dentry_t dentry_t;
typedef struct inode_t inode_t;
typedef struct boot_block_t boot_block_t;
typedef struct data_block_t data_block_t;
typedef struct  fop_table fop_table;

//***************************************************************************************************************
typedef struct pcb_t pcb_t;
typedef struct fd_t fd_t;
#endif /* ASM */

#endif /* _TYPES_H */
