#ifndef ASM
#include "x86_desc.h"



#define keyboard_index 0x21
#define rtc_index 0x28

extern void INT_0();
extern void INT_1(); 
extern void INT_2(); 
extern void INT_3(); 
extern void INT_4(); 
extern void INT_5(); 
extern void INT_6(); 
extern void INT_7(); 
extern void INT_8(); 
extern void INT_9(); 
extern void INT_10(); 
extern void INT_11(); 
extern void INT_12(); 
extern void INT_13(); 
extern void INT_14(); 
extern void INT_15(); 
extern void INT_16(); 
extern void INT_17(); 
extern void INT_18(); 
extern void INT_19(); 
extern void SYSCALL();

extern void idt_init();
extern void SYSCALL();
#endif
