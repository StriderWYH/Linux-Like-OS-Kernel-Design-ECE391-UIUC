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

void execute_test();
//check point 2
void terminal_test();
void file_read_testsf();
void file_read_testexe();
void file_read_testlf();
void print_out_all_files();
//int rtc_test();
void rtc_test();

//checkpoint 3
void systemcall_rtc_test();
void systemcall_terminal_test();
// CP3
void r_file_offset();
void r_w_test_smfile();
void oc_test();
void read_dir();
#endif /* TESTS_H */

