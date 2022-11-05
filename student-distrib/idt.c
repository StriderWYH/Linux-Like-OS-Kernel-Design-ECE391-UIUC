#include "x86_desc.h"
#include "idt.h"
//#include "idt_handler.h"
#include "rtc.h"
#include "keyboard.h"
#include "lib.h"
#include "systemcall.h"

#include "key_sd.h"
/*
idt_init;
idt_init initialization thw whole idt table, and set the entry for all the interrupt and exception, and system call
Input : none
Output: none
*/
#define KERNEL_STACK_OF 4


void 
idt_init ()
{
    int i; // loop index
    for (i = 0; i < NUM_VEC; i++) {
        idt[i].seg_selector = KERNEL_CS;  // set the related bits for the idt_entry
        idt[i].present = 0;
        idt[i].size = 1;
        idt[i].reserved0 = 0;
        idt[i].reserved1 = 1;
        idt[i].reserved2 = 1;
        idt[i].dpl = i == 0x80? 3:0;  // only for system call, the DPL should be 3
        if (i != 15 || i < 32){ // for the index 2 and index from 32-255 , which is the interrrupt, which has the different entry format
            idt[i].reserved3 = 1;           //except 15 is of no use, so we ingnore it
            idt[i].present = 1;
        }
        else{                            
            idt[i].reserved3 = 0;  // for interrupt GATE they should be zero
        }
        idt[i].reserved4 = 0;        
    }
    idt[0x21].present = 1;  // additionally, for the entry that is used for special cass, we individually use them to be present
    idt[0x28].present = 1;  //0x21 is for keyboard   0x28 is for rtc   0x80 is or system call
    idt[0x80].present = 1;

    SET_IDT_ENTRY (idt[0], INT_0);    // FILL THE ENTRY INTO THE IDT table, according to the interrup and exception defined in the IA-32 refer - INTEL
    SET_IDT_ENTRY (idt[1], INT_1);
    SET_IDT_ENTRY (idt[2], INT_2);   // warning, for these 0-20 exception and interrupt, i didnot use the wrp function for them, which should be filled in later checkpoints
    SET_IDT_ENTRY (idt[3], INT_3);
    SET_IDT_ENTRY (idt[4], INT_4);
    SET_IDT_ENTRY (idt[5], INT_5);
    SET_IDT_ENTRY (idt[6], INT_6);
    SET_IDT_ENTRY (idt[7], INT_7);
    SET_IDT_ENTRY (idt[8], INT_8);
    SET_IDT_ENTRY (idt[9], INT_9);
    SET_IDT_ENTRY (idt[10], INT_10);
    SET_IDT_ENTRY (idt[11], INT_11);
    SET_IDT_ENTRY (idt[12], INT_12);
    SET_IDT_ENTRY (idt[13], INT_13);
    SET_IDT_ENTRY (idt[14], INT_14);
    SET_IDT_ENTRY (idt[15], INT_15);
    SET_IDT_ENTRY (idt[16], INT_16);
    SET_IDT_ENTRY (idt[17], INT_17);
    SET_IDT_ENTRY (idt[18], INT_18);
    SET_IDT_ENTRY (idt[19], INT_19);

    SET_IDT_ENTRY (idt[keyboard_index], keyboard_handler);   // For keyboard_handler, we put the index into the index 22 in the table
    //printf("%d\n",idt[0x21].offset_15_00);
    //printf("%d\n",idt[0x21].offset_31_16);
    //printf("%d\n",idt[0x21].present);
    SET_IDT_ENTRY (idt[rtc_index], rtc_handler);  // For RTC handler, we put index into the index 24 in the table
    
    SET_IDT_ENTRY (idt[0X80], SYSCALL);   // for system call, we put the index into the index 0x80 in the table

}


/*  void INT_0-19()
 *    DESCRIPTION: The interrupt and exception handler program
 *  input /output : none 
 *  side effect: execute the interrupt handling program
 * */


void INT_0() 
{
    cli();
    printf ("Divide Error \n");
    halt(KERNEL_STACK_OF);
	while(1);
}
void INT_1() 
{
    cli();
    printf ("Debug Exception \n");
    halt(KERNEL_STACK_OF);
	while(1);
}
void INT_2() 
{
    cli();
    printf ("NMI Interrupt \n");
    halt(KERNEL_STACK_OF);
	while(1);
}
void INT_3() 
{
    cli();
    printf ("Breakpoint exception \n");
    halt(KERNEL_STACK_OF);
	while(1);
}
void INT_4() 
{
    cli();
    printf ("Overflow exception \n");
    halt(KERNEL_STACK_OF);
	while(1);
}
void INT_5() 
{
    cli();
    printf ("Bound_range_exceed exception \n");
    halt(KERNEL_STACK_OF);
	while(1);
}
void INT_6() 
{
    cli();
    printf ("Invalid opcode exception \n");
    halt(KERNEL_STACK_OF);
	while(1);
}
void INT_7() 
{
    cli();
    printf ("DeviceNotApplicable exception \n");
    halt(KERNEL_STACK_OF);
	while(1);
}
void INT_8() 
{
    cli();
    printf ("Double Fault exception \n");
    halt(KERNEL_STACK_OF);
	while(1);
}
void INT_9() 
{
    cli();
    printf ("Coprocessor Segment Overrun exception \n");
    halt(KERNEL_STACK_OF);
	while(1);
}
void INT_10() 
{
    cli();
    printf ("Invalid TSS Exception \n");
    halt(KERNEL_STACK_OF);
	while(1);
}
void INT_11() 
{
    cli();
    printf ("Segment Not Present exception \n");
    halt(KERNEL_STACK_OF);
	while(1);
}
void INT_12() 
{
    cli();
    printf ("Stack Fault exception \n");
    halt(KERNEL_STACK_OF);
	while(1);
}
void INT_13() 
{
    cli();
    printf ("General Protection exception \n");
    halt(KERNEL_STACK_OF);
	while(1);
}
void INT_14() 
{
    cli();
    printf ("Page-Fault exception \n");
    halt(KERNEL_STACK_OF);
	while(1);
}
void INT_15() {
    cli();
    printf ("(Intel reserved. Do not use.) \n");
    halt(KERNEL_STACK_OF);
    while(1);
}
void INT_16() 
{
    cli();
    printf ("x87 FPU Floating-Point Error \n");
    halt(KERNEL_STACK_OF);
	while(1);
}
void INT_17() 
{
    cli();
    printf ("Alignment Check Exception \n");
    halt(KERNEL_STACK_OF);
	while(1);
}
void INT_18() 
{
    cli();
    printf ("Machine-Check Exception \n");
    halt(KERNEL_STACK_OF);
	while(1);
}
void INT_19() 
{
    cli();
    printf ("SIMD Floating-Point Exception \n");
    halt(KERNEL_STACK_OF);
	while(1);
}


void SYSCALL() 
{
    cli();
    printf ("SYSTEM CALLED \n");
	while(1);
}

