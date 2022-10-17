#ifndef _RTC_H
#define _RTC_H

#include "types.h"

#define RTC_IRQ     8
#define RTC_PORT    0x70
#define RTC_DATA_PORT   0x71

#define REGISTER_A  0x8A
#define REGISTER_B  0x8B
#define REGISTER_C  0x8C
#define REGISTER_D  0x8D

extern void rtc_init();
extern void rtc_interrupt_handler();
extern int get_interrupt_rate(int frequency);
extern void set_frequency(int rate);
#endif
