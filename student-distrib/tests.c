#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "rtc.h"
#include "i8259.h"
#include "keyboard.h"
#include "file_sys.h"
#include "systemcall.h"

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
	int32_t fd = 0;
	int32_t nbyte;
	clean_screen();
	buffer_1[0] = 2;
	RTC_open(NULL);
	while(1){
		
		RTC_write(fd,buffer_1,nbyte);
		RTC_read(fd,buffer_1,nbyte);
		index_ds++;
		putc(122);
		//print_stuff(122,index_ds);
		if((buffer_1[0] < 1024) && (i % 20 == 0))
		{
			buffer_1[0] = (buffer_1[0]) * 2;
		}
		i++;
	}
	RTC_close(fd);
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
	//terminal_open((uint8_t*)"stdin");
	while(1){
		int write;
		while(!keyboard_flag); 
        write = terminal_read(0,keyboard_buffer,global_keyboard_index);
        terminal_write(0,terminal_buffer,write);
		//terminal_read();
	}    
	//sterminal_close(0);

}


/*
 * file_read_testsf()
 * Description: test for read small file (e.g. frame0.txt).
 * INPUT: NONE
 * OUTPUT: NONE
 * Return Value: fail (warning sentence) or sucess (nothing)
 * Side Effects: clean the screen
 */
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
/*
 * file_read_testexe()
 * Description: test for read exe file (e.g. ls.txt).
 * INPUT: NONE
 * OUTPUT: NONE
 * Return Value: fail (warning sentence) or sucess (nothing)
 * Side Effects: clean the screen
 */
void file_read_testexe(){
	int32_t result,i,length;
	i = 0;
	clean_screen();
	result = file_open((uint8_t*)"ls");
	length = ((inode_t*)(boot_block + 1 + glob_dentry.inode_num))->length_of_file;
	uint8_t buf[length];
	file_close(0);
	if(result == -1){
		printf("fail opening ls\n");
		return;
	}
	result = file_read(0,buf,length);
	while(i < length){
		if(buf[i] != '\0'){
		//print_stuff(buf[i],i);
		putc(buf[i]);
		}
		i++;
	}
	printf("\n");
	printf("file_name: ls");
}
/*
 * file_read_testlf()
 * Description: test for read large file (e.g. ls.txt).
 * INPUT: NONE
 * OUTPUT: NONE
 * Return Value: fail (warning sentence) or sucess (nothing)
 * Side Effects: clean the screen
 */
void file_read_testlf(){
	int32_t result,i,length;
	i = 0;
	clean_screen();
	result = file_open((uint8_t*)"verylargetextwithverylongname.tx");
	length = ((inode_t*)(boot_block + 1 + glob_dentry.inode_num))->length_of_file;
	uint8_t buf[length];
	file_close(0);
	if(result == -1){
		printf("fail opening verylargetextwithverylongname.tx\n");
		return;
	}
	result = file_read(0,buf,length);
	while(i < length){
		if(buf[i] != '\0'){
		//print_stuff(buf[i],i);
		putc(buf[i]);
		}
		i++;
	}
	printf("\n");
	printf("file_name: verylargetextwithverylongname.tx");
} 

/*
 * print_out_all_files()
 * Description: print out all the filename, corresponding file type and file name in the current file system.
 * INPUT: NONE
 * OUTPUT: NONE
 * Return Value: fail (warning sentence) or sucess (nothing)
 * Side Effects: clean the screen
 */
void print_out_all_files(){
	uint32_t result,file_size,filename_length,i;
	uint8_t buf[5349];
	uint8_t filename_buf[33];  // used for print ou the filename with fix format
	/* initialize the filname_buf */
	filename_buf[32] = '\0';
	for(i=0;i<32;i++){
			filename_buf[i] = ' ';
	}
	clean_screen();
	result = dir_open((uint8_t*)"."); // try open the dir
	if(result == -1){
		printf("fail opening .\n");
		return;
	}
	// if open successfully, print all the filename and corresponding file type and file size in the fix format
	while(!dir_read(0,buf,0)){
		// print file name
		printf("file_name: ");
		filename_length = strlen(glob_dentry_for_dirread.filename);
		if(filename_length > 32) filename_length = 32;
		for(i = 0; i <filename_length ;i++){
			filename_buf[32-filename_length+i] = glob_dentry_for_dirread.filename[i];
		}
		puts((int8_t*)filename_buf);
		// print file type
		printf(", file_type: ");
		printf("%d, ",glob_dentry_for_dirread.filetype);
		file_size = ((inode_t*)boot_block + 1 + glob_dentry_for_dirread.inode_num)->length_of_file;
		// print file size
		printf("file_size:");
		if(file_size < 10) printf("      ");
		else if (file_size < 100)
		{
			printf("     ");/* 5 spaces */
		}
		else if (file_size < 1000)
		{
			printf("    ");/* 4 spaces */
		}
		else if (file_size < 10000)
		{
			printf("   ");/* 3 spaces */
		}
		else if (file_size < 100000)
		{
			printf("  ");/* 2 spaces */
		}
		else{
			printf(" ");/* 1 spaces */
		}
		printf("%d",file_size);
		printf("\n");
		// clear the filename buf for the next file
		for(i=0;i<32;i++){
			filename_buf[i] = ' ';
		}
	}

}
/* Checkpoint 3 tests */
void systemcall_rtc_test() 
{
	TEST_HEADER;
	int i = 0;
	int index_ds = 0;
	int buffer_1[1];
	int32_t fd = open((uint8_t*)"rtc");
	int32_t result = 0;
	int32_t nbyte = 0;
	clean_screen();

	printf("fd is : %d \n",fd);
	int esp;
    // fetch the address of the current PCB
    asm("movl %%esp, %0" : "=r"(esp) :);
    pcb_t *pcb = (pcb_t *)( esp & PCB_MSK);
	uint32_t flag = pcb->file_array[fd].flags;
	printf("flag is : %d\n",flag);

	buffer_1[0] = 2;
	if (fd==-1) {return;}
	RTC_open((uint8_t*)"rtc");
	while(1){
		
		write(fd,buffer_1,nbyte);
		read(fd,buffer_1,nbyte);
		index_ds++;
		putc(122);
		//print_stuff(122,index_ds);
		if((buffer_1[0] < 1024) && (i % 20 == 0))
		{
			buffer_1[0] = (buffer_1[0]) * 2;
		}
		i++;
	}
	result = close(fd);
	//printf("result is : %d\n",result);
	//rtc_interrupt_handler();

	//return PASS;
}

void systemcall_terminal_test()
{
	int32_t fd = open((uint8_t*)"stdin");
	int32_t result = 0;
	terminal_open(NULL);
	while(1){
		int systemcall_terminal_flag;
		while(!keyboard_flag); 
        systemcall_terminal_flag = read(fd,keyboard_buffer,global_keyboard_index);
        write(fd,terminal_buffer,systemcall_terminal_flag);
		//terminal_read();
	}    
	terminal_close(0);
	result = close(fd);
}




void execute_test(){
	printf("execute test: shell doc");
	execute((uint8_t *)"shell");
}


void oc_test(){
	int result;
	int esp;
	result = open( (uint8_t*)"stdin");
	if( result != 0){
		printf(" fail to assaign fd 0 to stdin\n");
		return;
	}
	result = open( (uint8_t*)"stdout");
	if( result != 1){
		printf(" fail to assaign fd 1 to stdout\n");
		return;
	}
	result = open( (uint8_t*) "frame0.txt");
	if( result != 2){
		printf(" fail to assaign fd 2 to frame0.txt\n");
		return;
	}
	result = open( (uint8_t*) "verylargetextwithverylongname.tx");
	if( result != 3){
		printf(" fail to assaign fd 3 to verylargetextwithverylongname.tx\n");
		return;
	}
	result = open((uint8_t*) "nosuchafile");
	if( result != -1){
		printf(" open() fails to return -1 when open a non-existing file\n");
		return;
	}
    // fetch the address of the current PCB
    asm("movl %%esp, %0" : "=r"(esp) :);
    pcb_t *pcb = (pcb_t *)( esp & PCB_MSK);

	if(pcb->file_array[0].flags == 0){
		printf("fail opening the stdin\n");
		return;		
	}

	if(pcb->file_array[1].flags == 0){
		printf("fail opening stdout\n");
		return;
	}

	if(pcb->file_array[2].flags == 0){
		printf("fail opening frame0.txt\n");
		return;
	}
	if(pcb->file_array[3].flags == 0){
		printf("fail opening verylargetextwithverylongname.tx\n");
		return;
	}
	if(pcb->file_array[4].flags == 1){
		printf("open a not existing file\n");
		return;
	}

	result = close(0);
	if( result != -1){
		printf(" we should not allow close stdin\n");
		return;
	}
	result = close(1);
	if( result != -1){
		printf(" we should not allow close stdout\n");
		return;
	}
	result = close(2);
	if(pcb->file_array[2].flags == 1){
		printf("fail closing frame0.txt\n");
		return;
	}
	result = close(3);
	if(pcb->file_array[3].flags == 1){
		printf("fail closing verylargetextwithverylongname.tx\n");
		return;
	}
	result = close(4);
	if( result != -1){
		printf(" we should not allow close a null file\n");
		return;
	}
	printf("open and close all success \n");
	return;
}

// test read and write function for txt files without offset
void r_w_test_smfile(){
	uint8_t buf[500];
	int32_t fd_cur,i,result;
	i = 0;
	clean_screen();
	fd_cur = open((uint8_t*)"frame0.txt");
	if(fd_cur == -1){
		printf("fail opening frame0.txt\n");
		return;
	}
	read(fd_cur,buf,500);
	while((buf[i]) != '\0'){
		putc(buf[i]);
		i++;
	}
	result = write(fd_cur,buf,500);
	if(result != -1){
		printf(" We should not allow write into a txt file\n");
	}
	
	printf("\n");
	printf("file_name: fram0.txt");
	close(fd_cur);
	
	return;

}
// read a file, include offset
void r_file_offset(){
	int32_t result,i;
	i = 0;
	clean_screen();
	result = open((uint8_t*)"frame0.txt");
	if(result == -1){
		printf("fail opening frame0.txt\n");
		return;
	}

	uint8_t buf[10];
	read(result,buf,10);
	while(i < 10){
		if(buf[i] != '\0'){
		//print_stuff(buf[i],i);
		putc(buf[i]);
		}
		i++;
	}

	uint8_t buf2[10];
	read(result,buf2,10);
	while(i < 10){
		if(buf2[i] != '\0'){
		//print_stuff(buf[i],i);
		putc(buf2[i]);
		}
		i++;
	}
	printf("\n");
	printf("all success");
	return;
}

/* Checkpoint 4 tests */
/* Checkpoint 5 tests */

/* Test suite entry point */
void launch_tests(){


	//div_test();
	//syscall_test();
	//rtc_test();
	//systemcall_rtc_test();
	//TEST_OUTPUT("idt_test", idt_test());
	//TEST_OUTPUT("rtc_idt_entry test",idt_special_test_forRtc());
	//TEST_OUTPUT("Keyboard_idt_entry test",idt_special_test_forKey());


	//null_test();
	//TEST_OUTPUT("VirtualMemory_paging_test", VirtualMemory_paging_test());
	//TEST_OUTPUT("VirtualMemory_paging_out_test()", VirtualMemory_paging_out_test());

	//TEST_OUTPUT("Kernel_paging_test", Kernel_paging_test());
	//TEST_OUTPUT("kernel_paging_out_test", kernel_paging_out_test());

	//PagingFault_test();
	//terminal_test();
	//systemcall_terminal_test();
	//file_read_testsf();
	//file_read_testexe();
	//file_read_testlf();
	//execute_test();
	//r_file_offset();
	//r_w_test_smfile();
	//oc_test();
	//print_out_all_files();
	// launch your tests here
}
