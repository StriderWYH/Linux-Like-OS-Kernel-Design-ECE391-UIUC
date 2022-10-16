#ifndef _I8259_H
#define _I8259_H

#include "types.h"

#define RTC_IRQ     8
#define RTC_PORT    0x70
#define CMOS_PORT   0x71

#define REGISTER_A  0x8A
#define REGISTER_B  0x8B
#define REGISTER_C  0x8C
#define REGISTER_D  0x8D

void rtc_init();
void rtc_interrupt_handler();
// int get_interrupt_rate(int frequency);

#endif