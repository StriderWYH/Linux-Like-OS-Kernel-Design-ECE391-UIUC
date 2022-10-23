#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "rtc.h"
#include "i8259.h"
#include "keyboard.h"
#include "file_sys.h"
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
 * void rtc_test()
 * Description: This function call the rtc read and write and set frequency every 20 characters
 * Return Value: none
 * Side Effects: rtc interrupts enabled
 */
 
void rtc_test() 
{
	TEST_HEADER;
	int i = 0;
	int index_ds = 0;
	int buffer_1[1];
	clean_screen();
	buffer_1[0] = 2;
	RTC_open();
	while(1){
		
		RTC_write(buffer_1);
		RTC_read();
		index_ds++;
		print_stuff(122,index_ds);
		if((buffer_1[0] < 1024) && (i % 20 == 0))
		{
			buffer_1[0] = (buffer_1[0]) * 2;
		}
		i++;
	}
	RTC_close();
	//rtc_interrupt_handler();

	//return PASS;
}


/* Checkpoint 2 tests */

/* terminal_test()
 * introduction: continusly call the read and write to test them
 * input: none
 * output: none
 */
void terminal_test(){
	terminal_open(0);
	while(1){
		int write;
        write = terminal_read(global_keyboard_index);
        terminal_write(write);
		//terminal_read();
	}    
	terminal_close(0);

}


void file_read_testsf(){
	uint8_t buf[500];
	int32_t result,i;
	i = 0;
	clean_screen();
	result = file_open((uint8_t*)"frame0.txt");
	file_close(0);
	if(result == -1){
		printf("fail opening frame0.txt\n");
		return;
	}
	result = file_read(0,buf,500);
	while((buf[i]) != '\0'){
		putc(buf[i]);
		i++;
	}
	printf("\n");
	printf("file_name: fram0.txt");
}

void file_read_testexe(){
	uint8_t buf[5349];
	int32_t result,i;
	i = 0;
	clean_screen();
	result = file_open((uint8_t*)"ls");
	file_close(0);
	if(result == -1){
		printf("fail opening ls\n");
		return;
	}
	result = file_read(0,buf,5349);
	while(i < 5349){
		print_stuff(buf[i],i);
		i++;
	}
	printf("\n");
	printf("file_name: ls");
}

void file_read_testlf(){
	uint8_t buf[5349];
	int32_t result,i;
	i = 0;
	clean_screen();
	result = file_open((uint8_t*)"verylargetextwithverylongname.tx");
	file_close(0);
	if(result == -1){
		printf("fail opening verylargetextwithverylongname.tx\n");
		return;
	}
	result = file_read(0,buf,5349);
	while(i < 5349){
		print_stuff(buf[i],i);
		i++;
	}
	printf("\n");
	printf("file_name: verylargetextwithverylongname.tx");
} 

void print_out_all_files(){
	uint32_t result,file_size,filename_length,i;
	uint8_t buf[5349];
	uint8_t filename_buf[33];
	filename_buf[32] = '\0';
	for(i=0;i<32;i++){
			filename_buf[i] = ' ';
		}
	clean_screen();
	result = dir_open((uint8_t*)".");
	if(result == -1){
		printf("fail opening .\n");
		return;
	}
	while(!dir_read(0,buf,0)){
		printf("file_name: ");
		filename_length = strlen(glob_dentry_for_dirread.filename);
		if(filename_length > 32) filename_length = 32;
		for(i = 0; i <filename_length ;i++){
			filename_buf[32-filename_length+i] = glob_dentry_for_dirread.filename[i];
		}
		puts((int8_t*)filename_buf);
		printf(", file_type: ");
		printf("%d, ",glob_dentry_for_dirread.filetype);
		file_size = ((inode_t*)boot_block + 1 + glob_dentry_for_dirread.inode_num)->length_of_file;
		printf("file_size:");
		if(file_size < 10) printf("      ");
		else if (file_size < 100)
		{
			printf("     ");/* code */
		}
		else if (file_size < 1000)
		{
			printf("    ");/* code */
		}
		else if (file_size < 10000)
		{
			printf("   ");/* code */
		}
		else if (file_size < 100000)
		{
			printf("  ");/* code */
		}
		else{
			printf(" ");/* code */
		}
		printf("%d",file_size);
		printf("\n");
		for(i=0;i<32;i++){
			filename_buf[i] = ' ';
		}
	}

}
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){


	//div_test();
	//syscall_test();
	//rtc_test();

	//TEST_OUTPUT("idt_test", idt_test());
	//TEST_OUTPUT("rtc_idt_entry test",idt_special_test_forRtc());
	//TEST_OUTPUT("Keyboard_idt_entry test",idt_special_test_forKey());


	//null_test();
	//TEST_OUTPUT("VirtualMemory_paging_test", VirtualMemory_paging_test());
	//TEST_OUTPUT("VirtualMemory_paging_out_test()", VirtualMemory_paging_out_test());

	//TEST_OUTPUT("Kernel_paging_test", Kernel_paging_test());
	//TEST_OUTPUT("kernel_paging_out_test", kernel_paging_out_test());

	//PagingFault_test();
	terminal_test();
	//file_read_testsf();
	//file_read_testexe();
	//file_read_testlf();
	//print_out_all_files();
	// launch your tests here
}
