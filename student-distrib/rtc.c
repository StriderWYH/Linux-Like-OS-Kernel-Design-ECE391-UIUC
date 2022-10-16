#include "rtc.h"
#include "lib.h"
#include "i8259.h"





void rtc_init()
{
    unsigned int temp;
    disable_irq(RTC_IRQ);   //no interrupts happen
    outb(REGISTER_B,RTC_PORT);  //select Status Register B, and disable NMI
    temp = inb(CMOS_PORT);  // read the current value of register B
    outb(REGISTER_B,RTC_PORT);  // set the index again (a read will reset the index to register D)
    outb(temp|0x40,CMOS_PORT);  // write the previous value ORed with 0x40. This turns on bit 6 of register B
    enable_irq(RTC_IRQ);

}

void rtc_interrupt_handler()
{
    send_eoi(RTC_IRQ);
    disable_irq(RTC_IRQ);
    outb(REGISTER_C,RTC_PORT);  //select register C
    inb(CMOS_PORT); //throw away content
    enable_irq(RTC_IRQ);
    printf("rtc_handler succuss!")
}

// /*
//  * frequency = 32768 >> (rate-1)   https://wiki.osdev.org/RTC
//  */
// int get_interrupt_rate(int frequency)
// {
//     if (frequency == 2) {return 15;}
//     if (frequency == 4) {return 14;}
//     if (frequency == 8) {return 13;}
//     if (frequency == 16) {return 12;}
//     if (frequency == 32) {return 11;}
//     if (frequency == 64) {return 10;}
//     if (frequency == 128) {return 9;}
//     if (frequency == 256) {return 8;}
//     if (frequency == 512) {return 7;}
//     if (frequency == 1024) {return 6;}
//     return -1;  //return -1 for fail
// }