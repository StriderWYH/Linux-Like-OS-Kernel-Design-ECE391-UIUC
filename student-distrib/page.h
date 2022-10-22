#ifndef _page_H
#define _page_H


#include "x86_desc.h"
#include "lib.h"

#define video_memory    0xb8000
#define kernel          0x400000

// structure of PDE_MB entry
struct PDE_4MB
{ 
    union{
    uint32_t value;
    struct {
        uint32_t present                        :1;
        uint32_t R_W                            :1;
        uint32_t user_or_Supervisor             :1;
        uint32_t write_through                  :1;
        uint32_t cache_disabled                 :1;
        uint32_t accessed                       :1;
        uint32_t dirty                          :1;
        uint32_t page_size                      :1;
        uint32_t global_page                    :1;
        uint32_t availablr_for_sysuser          :3;
        uint32_t page_table_index               :1;
        uint32_t reserved                       :9;
        uint32_t table_base_add                 :10;
    } __attribute__ ((packed));
};  
};

// structure of PDE_KB entry
struct PDE_4KB
{ 
    union{
    uint32_t value;
    struct {
        uint32_t present                        :1;
        uint32_t R_W                            :1;
        uint32_t user_or_Supervisor             :1;
        uint32_t write_through                  :1;
        uint32_t cache_disabled                 :1;
        uint32_t accessed                       :1;
        uint32_t reserved                       :1;
        uint32_t page_size                      :1;
        uint32_t global_page                    :1;
        uint32_t availablr_for_sysuser          :3;
        uint32_t table_base_add                 :20;
    } __attribute__ ((packed));
};  
};

// Union the MB and KB PDE to creat the table type
union PDE
{
    PDE_4KB KBPDE;
    PDE_4MB MBPDE;
    /* data */
};

// structure of PTE entry
struct PTE
{ 
    union{
    uint32_t value;
    struct {
        uint32_t present                        :1;
        uint32_t R_W                            :1;
        uint32_t user_or_Supervisor             :1;
        uint32_t write_through                  :1;
        uint32_t cache_disabled                 :1;
        uint32_t accessed                       :1;
        uint32_t dirty                          :1;
        uint32_t page_table_index               :1;
        uint32_t global_page                    :1;
        uint32_t available_for_sysuser          :3;
        uint32_t table_base_add                 :20;
    } __attribute__ ((packed));
};  
};



extern PTE PTE_TABLE[num_PTE];
extern PDE PDE_TABLE[num_PDE];      // creat the PDE and PTE table
extern int32_t PDE_TABLE_bottom;
extern int32_t PTE_TABLE_bottom;       //point to the bottom
extern void page_init();

#endif /* _page_H */

