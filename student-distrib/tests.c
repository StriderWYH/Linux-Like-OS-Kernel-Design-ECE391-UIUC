#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "rtc.h"
#include "i8259.h"
#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

/* IDT Test - Example
 * 
 * Asserts that the special IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_special_test_forKey(){
	TEST_HEADER;
	int i = 0x21;
	int result = PASS;

	if ((idt[i].offset_15_00 == NULL) && 
		(idt[i].offset_31_16 == NULL)){
		assertion_failure();
		result = FAIL;
	}

	return result;
}

/* IDT Test - Example
 * 
 * Asserts that the special IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_special_test_forRtc(){
	TEST_HEADER;
	int i = 0x28;
	int result = PASS;

	if ((idt[i].offset_15_00 == NULL) && 
		(idt[i].offset_31_16 == NULL)){
		assertion_failure();
		result = FAIL;
	}
	
	return result;
}

// add more tests here
/* 
 * void null_test()
 * Description: This function deref a null pointer, test whether paging works
 * Inputs: none
 * Outputs: 
 * Side Effects: Kernel freeze.
 */
void null_test(){
	TEST_HEADER;
	int* ptr = NULL;
	int PF;
	printf("loading the NULL test for Paging function\n");
	PF = *(ptr);
	printf("The pointer of NULL is %d , The NULL test fails if it is not -1",&PF);
}


/* 
 * void div_test()
 * Description: This function test div exception is loaded.
 * Inputs: none
 * Outputs: 
 * Side Effects: Kernel freeze.
 */
void div_test(){
	TEST_HEADER;
	printf("Testing the Div exception: let 1/0\n ");
	int a = 1;
	int b = 0;
	a = a/b;
}
/* 
 * void PagingFault_test()
 * Description: This function deref an uninitialized page.
 * Inputs: none
 * Outputs: PagingFault
 * Side Effects: Kernel freeze.
 */
void PagingFault_test(){
	TEST_HEADER;
	int* ptr = (int*)(0x800000 + 1);
	int p;
	p = *(ptr);
}


/* 
 * int VirtualMemory_paging_test()
 * Description: This function deref an addr in vidmem.
 * Inputs: none
 * Outputs: none
 * Return Value: PASS 1/ FAIL 0
 * Side Effects: none
 */
int VirtualMemory_paging_test(){
	TEST_HEADER;
	int * ptr = (int*)(0xB8000 + 1);
	int Virtual;
	Virtual = *ptr;
	return PASS;
}

/* 
 * int VirtualMemory_paging_out_test()
 * Description: This function deref an addr in vidmem.
 * Inputs: none
 * Outputs: none
 * Return Value: PASS 1/ FAIL 0
 * Side Effects: none
 */
int VirtualMemory_paging_out_test(){
	TEST_HEADER;
	printf("loading virtualmemory out test: This should fail!\n");
	int * ptr = (int*)(0xB8000 - 1);
	int Virtual;
	Virtual = *ptr;
	return PASS;
}

/* 
 * int Kernel_paging_test()
 * Description: This function deref an addr in kernel.
 * Inputs: none
 * Outputs: none
 * Return Value: PASS 1/ FAIL 0
 * Side Effects: none
 */
int Kernel_paging_test(){
	TEST_HEADER;
	int * ptr = (int*)(0x400000 + 1);
	int Kernel;
	Kernel = *ptr;
	return PASS;
}

/* 
 * int kernel_paging_out_test()
 * Description: This function deref an addr in vidmem.
 * Inputs: none
 * Outputs: none
 * Return Value: PASS 1/ FAIL 0
 * Side Effects: none
 */
int kernel_paging_out_test(){
	TEST_HEADER;
	printf("loading kernel out test: This should fail!\n");
	int * ptr = (int*)(0x400000 - 1);
	int Kernel;
	Kernel = *ptr;
	return PASS;
}

/* Sys call test
 * 
 * Tests is syscalls are accounted for
 * Inputs: None
 * Outputs: None
 * Side Effects: Should print out syscall handler
 * Coverage: Exception Handler
 * Files: idt.c, ex_handler.c
 */
void syscall_test() {
	TEST_HEADER;
	asm volatile ("\
		movl $40, %eax\n\
		int $0x80\n\
	");
}

/*
 * int rtc_test()
 * Description: This function tests rtc interrupt handler.
 * Return Value: PASS
 * Side Effects: rtc interrupts enabled
 */
 
int rtc_test() {
	TEST_HEADER;
	rtc_interrupt_handler();

	return PASS;
}


/* Checkpoint 2 tests */
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){


	//div_test();
	//syscall_test();


	//TEST_OUTPUT("idt_test", idt_test());
	//TEST_OUTPUT("rtc_idt_entry test",idt_special_test_forRtc());
	//TEST_OUTPUT("Keyboard_idt_entry test",idt_special_test_forKey());


	//null_test();
	//TEST_OUTPUT("VirtualMemory_paging_test", VirtualMemory_paging_test());
	//TEST_OUTPUT("VirtualMemory_paging_out_test()", VirtualMemory_paging_out_test());

	//TEST_OUTPUT("Kernel_paging_test", Kernel_paging_test());
	//TEST_OUTPUT("kernel_paging_out_test", kernel_paging_out_test());

	PagingFault_test();

	// launch your tests here
}
