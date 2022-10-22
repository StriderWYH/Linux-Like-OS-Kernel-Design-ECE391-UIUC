#include "rtc.h"
#include "lib.h"
#include "i8259.h"

volatile int RTC_interrupt;

/*
 * introduction: initialize the rtc
 * input: none
 * output: none
 */
void rtc_init()
{
    char temp;
    disable_irq(RTC_IRQ);   //no interrupts happen
    outb(REGISTER_B,RTC_PORT);  //select Status Register B, and disable NMI
    temp = inb(RTC_DATA_PORT);  // read the current value of register B
    outb(REGISTER_B,RTC_PORT);  // set the index again (a read will reset the index to register D)
    outb(temp|0x40,RTC_DATA_PORT);  // write the previous value ORed with 0x40. This turns on bit 6 of register B
    enable_irq(RTC_IRQ);
}

/*
 * introduction: create a handler for idt to call 
 * input: none
 * output: none
 */
void rtc_interrupt_handler()
{
    
    outb(REGISTER_C,RTC_PORT);  //select register C
    inb(RTC_DATA_PORT); //throw away content
    RTC_interrupt = 1;
    //test_interrupts();
    send_eoi(RTC_IRQ);
}

/*
 * introduction: set the frequency to the RTC 
 * input: int rate
 * output: none
 */
void set_frequency(int rate)
{
    char temp;
    rate = rate & 0x0F; //low 4 bits
    //disable_irq(RTC_IRQ);
    outb(REGISTER_A,RTC_PORT);
    temp = inb(RTC_DATA_PORT);
    outb(REGISTER_A,RTC_PORT);
    temp = temp & 0xF0;
    outb(temp + rate,RTC_DATA_PORT);
    //enable_irq(RTC_IRQ);
}



/*
 * frequency = 32768 >> (rate-1)   https://wiki.osdev.org/RTC
 */
 /*
 * introduction: calculate the rate
 * input: int frequency
 * output: int rate
 */
int get_interrupt_rate(int frequency)
{
    if (frequency == 2) {return 15;}
    if (frequency == 4) {return 14;}
    if (frequency == 8) {return 13;}
    if (frequency == 16) {return 12;}
    if (frequency == 32) {return 11;}
    if (frequency == 64) {return 10;}
    if (frequency == 128) {return 9;}
    if (frequency == 256) {return 8;}
    if (frequency == 512) {return 7;}
    if (frequency == 1024) {return 6;}
    return -1;  //return -1 for fail
}




int RTC_open()
{
    int frequency = 2;
    int rate = get_interrupt_rate(frequency);
    set_frequency(rate);
    return 0;
}

int RTC_close()
{
    return 0;
}

int RTC_read()
{
    while(RTC_interrupt == 0){}
    RTC_interrupt = 0;
    return 0;
}


int RTC_write(void* buffer)
{
    int frequency = *((int*)buffer);
    int rate = get_interrupt_rate(frequency);
    if (rate == -1)     //if the frequency is not the power of 2 and not >2,<1024, return -1
    {
        return -1;      //if fail to set frequency, return -1
    }
    set_frequency(rate);
    return 0;           //return 0 for success
}
