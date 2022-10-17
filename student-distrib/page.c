#include "page.h"
//#include "x86_desc.h"

#define size_of_4kb             0x1000
#define size_of_4mb             0x400000


/*
page_init;
page_init initialization the PDE table and PTE table, and set the register in system
Input : none
Output: none
*/
void page_init(){
    int32_t PDE_index;
    int32_t PTE_index;
    
    //set register
    
    
    PDE_TABLE[0].KBPDE.present = 1;
    PDE_TABLE[0].KBPDE.R_W = 1;
    PDE_TABLE[0].KBPDE.accessed = 0;
    PDE_TABLE[0].KBPDE.availablr_for_sysuser = 0;
    PDE_TABLE[0].KBPDE.cache_disabled = 0;
    PDE_TABLE[0].KBPDE.global_page = 0;
    PDE_TABLE[0].KBPDE.page_size = 0;
    PDE_TABLE[0].KBPDE.reserved = 0;
    PDE_TABLE[0].KBPDE.user_or_Supervisor = 0;
    PDE_TABLE[0].KBPDE.write_through = 0;
    PDE_TABLE[0].KBPDE.table_base_add = ((int32_t)&(PTE_TABLE[0])) >> 12;           // set the first PDE entry, it should be KBPDE, and set the arguement. 12 means the table base is the highest 20bit



    PDE_TABLE[1].MBPDE.present = 1;
    PDE_TABLE[1].MBPDE.R_W = 1;
    PDE_TABLE[1].MBPDE.accessed = 0;
    PDE_TABLE[1].MBPDE.availablr_for_sysuser = 0;
    PDE_TABLE[1].MBPDE.cache_disabled = 0;
    PDE_TABLE[1].MBPDE.global_page = 1;
    PDE_TABLE[1].MBPDE.page_size = 1;
    PDE_TABLE[1].MBPDE.availablr_for_sysuser = 0;
    PDE_TABLE[1].MBPDE.user_or_Supervisor = 1;
    PDE_TABLE[1].MBPDE.write_through = 0;
    PDE_TABLE[1].MBPDE.table_base_add = (kernel) >> 22;     //set the PDE of kernel MBPDE, it should be sudo view and is present, 22 is 22 bit that the address need to shift because the base address is only 10 bit


    for (PDE_index = 2; PDE_index < num_PDE; PDE_index++){

        PDE_TABLE[PDE_index].MBPDE.present = 0;
        PDE_TABLE[PDE_index].MBPDE.R_W = 1;
        PDE_TABLE[PDE_index].MBPDE.accessed = 0;
        PDE_TABLE[PDE_index].MBPDE.availablr_for_sysuser = 0;
        PDE_TABLE[PDE_index].MBPDE.cache_disabled = 0;
        PDE_TABLE[PDE_index].MBPDE.global_page = 0;
        PDE_TABLE[PDE_index].MBPDE.page_size = 1;
        PDE_TABLE[PDE_index].MBPDE.reserved = 0;
        PDE_TABLE[PDE_index].MBPDE.user_or_Supervisor = 0;
        PDE_TABLE[PDE_index].MBPDE.write_through = 0;
        PDE_TABLE[PDE_index].MBPDE.dirty = 0;
        PDE_TABLE[PDE_index].MBPDE.page_table_index = 0;    //set the PDE of other MBPDE, they are not used so they are not present, 22 is 22 bit that the address need to shift because the base address is only 10 bit
        PDE_TABLE[PDE_index].MBPDE.table_base_add = (((PDE_index) * size_of_4mb)) >> 22;
        }
   
    for (PTE_index = 0; PTE_index < num_PTE; PTE_index++){
        PTE_TABLE[PTE_index].accessed = 0;
        PTE_TABLE[PTE_index].available_for_sysuser = 0;
        PTE_TABLE[PTE_index].cache_disabled = 0;
        PTE_TABLE[PTE_index].dirty = 0;
        PTE_TABLE[PTE_index].global_page = 0;
        PTE_TABLE[PTE_index].page_table_index = 0;
        PTE_TABLE[PTE_index].present = 0;
        PTE_TABLE[PTE_index].R_W = 1;
        PTE_TABLE[PTE_index].user_or_Supervisor = 0;
        PTE_TABLE[PTE_index].write_through = 0;          //set the PTE of other PTE, they are not used so they are not present, 12 is 12 bit that the address need to shift because the base address is only 20 bit
        PTE_TABLE[PTE_index].table_base_add = (((PTE_index) * size_of_4kb)) >> 12;
        if(PTE_index * size_of_4kb == video_memory){
          PTE_TABLE[PTE_index].present = 1;  
          PTE_TABLE[PTE_index].user_or_Supervisor = 1;     //set the PTE if the PTE is the video memory. Then the memory is used so set its present to 1 and it is also sudo view
        }
    }   
    asm volatile(
        "movl %0, %%eax;"
        "movl %%eax, %%cr3 ;"           //set the table

        "movl %%cr4, %%eax;"
        "orl $0x00000010, %%eax;"       //set the cr4 register, the bit 4 is set to have MB and KB PDE. 0x00000010 is to set bit 4
        "movl  %%eax,  %%cr4 ;"

        "movl %%cr0, %%eax;"
        "orl $0x80000000, %%eax;"       //set the cr0 register, the bit 31 is set to turn on the paging. 0x8000000 is to set bit 31
        "movl  %%eax,  %%cr0 ;"
        :
        :"r"(PDE_TABLE)
        :"eax"
    );
}


