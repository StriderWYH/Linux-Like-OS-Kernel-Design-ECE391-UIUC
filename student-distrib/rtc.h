<<<<<<< HEAD
#ifndef _I8259_H
#define _I8259_H
=======
#ifndef _RTC_H
#define _RTC_H
>>>>>>> vva

#include "types.h"

#define RTC_IRQ     8
#define RTC_PORT    0x70
<<<<<<< HEAD
#define CMOS_PORT   0x71
=======
#define RTC_DATA_PORT   0x71
>>>>>>> vva

#define REGISTER_A  0x8A
#define REGISTER_B  0x8B
#define REGISTER_C  0x8C
#define REGISTER_D  0x8D

<<<<<<< HEAD
void rtc_init();
void rtc_interrupt_handler();
// int get_interrupt_rate(int frequency);

#endif
=======
extern void rtc_init();
extern void rtc_interrupt_handler();
extern int get_interrupt_rate(int frequency);
extern void set_frequency(int rate);
#endif
>>>>>>> vva
