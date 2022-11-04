#include "rtc.h"
#include "lib.h"
#include "i8259.h"

volatile int RTC_interrupt;
int time_counter;   //rtc virtualization
volatile int virtual_frequency;       //rtc virtualization
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
    if (time_counter == 1024/virtual_frequency)     //rtc virtualization
    {
        RTC_interrupt = 1;
    }
    outb(0x0C,RTC_PORT);  //select register C
    inb(RTC_DATA_PORT); //throw away content
    time_counter++;     //rtc virtualization
    if (time_counter > 1024/virtual_frequency)
    {
        time_counter = time_counter%(1024/virtual_frequency);       //rtc virtualization
    }

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


/* RTC_open
 * introduction: open the rtc, intialize it and set the frequency to 2hz
 * input: none
 * output: 0 for success ,can't fail
 */
int RTC_open(int nbytes)
{
    int frequency = 1024;
    int rate = get_interrupt_rate(frequency);
    RTC_interrupt = 0;
    time_counter = 0;
    virtual_frequency = 2;
    set_frequency(rate);    //set 2 hz
    return 0;
}

/* RTC_close
 * introduction: close the rtc
 * input: none
 * output: 0 for success ,can't fail
 */
int RTC_close(int nbytes)
{
    return 0;
}

/* RTC_read
 * introduction: read the rtc, wait until receive the RTC_interrupt signal
 * input: none
 * output: 0 for success ,can't fail
 */
int RTC_read(int32_t fd, void* buf, int32_t nbytes)
{
    while(RTC_interrupt == 0){}     //wait for the interrupt
    RTC_interrupt = 0;
    return 0;
}

/* RTC_write
 * introduction: write the rtc, set an other frequency for rtc
 * input: void* buffer (contains the frequency)
 * output: 0 for success, 1 for fail, if the frequency is not the power of 2, fail
 */
int RTC_write(int32_t fd, void* buf, int32_t nbytes)
{
    // int frequency;
    // int rate;
    // frequency = *((int*)buffer);    //get the new frequency
    // rate = get_interrupt_rate(frequency);
    // if (rate == -1)     //if the frequency is not the power of 2 and not >2,<1024, return -1
    // {
    //     return -1;      //if fail to set frequency, return -1
    // }
    // set_frequency(rate);
    // return 0;           //return 0 for success

    /*rtc virtualization*/
    virtual_frequency = *((int*)buf);
    time_counter = 0;
    return 0;
}
