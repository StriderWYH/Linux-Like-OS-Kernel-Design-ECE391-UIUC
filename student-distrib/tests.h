#ifndef TESTS_H
#define TESTS_H

// test launcher
void launch_tests();
int idt_test();
void null_test();
void div_test();
void PagingFault_test();
int VirtualMemory_paging_test();
int Kernel_paging_test();
void syscall_test();

int idt_special_test_forKey();
int idt_special_test_forRtc();


int VirtualMemory_paging_out_test();
int kernel_paging_out_test();


//check point 2
void terminal_test();
void file_read_test1();
//int rtc_test();
#endif /* TESTS_H */
