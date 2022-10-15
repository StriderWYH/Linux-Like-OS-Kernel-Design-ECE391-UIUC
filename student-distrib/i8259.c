/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void i8259_init(void) {
    outb(0xFF,0x21);    
    outb(0xFF,0xA1);    //stop all the interrupt
    
    outb(ICW1,MASTER_8259_PORT);
    outb(ICW2_MASTER,0x21);
    outb(ICW3_MASTER,0x21);
    outb(ICW4,0x21);
    outb(ICW1,SLAVE_8259_PORT);
    outb(ICW2_SLAVE,0xA1);
    outb(ICW3_SLAVE,0xA1);
    outb(ICW4,0xA1);

    outb(cached_21,0x21);
    outb(cached_A1,0xA1);
}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
    int temp = 0x01;
    int IRQ2_not = 0x04;    //0000 0100
    if ((irq_num < 0) || (irq_num > 15)){return;} //invalid irq_number
    /*in master PIC*/
    if ((irq_num >= 0) && (irq_num <= 7))
    {
        temp = temp << irq_num;
        temp = ~temp;
        master_mask = master_mask & temp;
        outb(master_mask,MASTER_8259_PORT);
        return;
    }
    /*in slave PIC*/
    if ((irq_num >= 8) && (irq_num <= 15))
    {
        /*master port*/
        master_mask = master_mask & (~IRQ2_not);
        outb(master_mask,MASTER_8259_PORT);
        /*slave port*/
        irq_num = irq_num - 8;  //irq_number in slave, master from 0~7
        temp = temp << irq_num;
        temp = ~temp;
        slave_mask = slave_mask & temp;
        outb(slave_mask,SLAVE_8259_PORT);
        return;
    }
}


/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
    int temp = 0x01;
    int IRQ2 = 0x04;
    if ((irq_num < 0) || (irq_num > 15)){return;} //invalid irq_number
    /*in master PIC*/
    if ((irq_num >= 0) && (irq_num <= 7))
    {
        temp = temp << irq_num;
        master_mask = master_mask | temp;
        outb(master_mask,MASTER_8259_PORT);
        return;
    }
    /*in slave PIC*/
    if ((irq_num >= 8) && (irq_num <= 15))
    {
        /*master port*/
        master_mask = master_mask | IRQ2;
        outb(master_mask,MASTER_8259_PORT);
        /*slave port*/
        irq_num = irq_num - 8;  //irq_number in slave, master from 0~7
        temp = temp << irq_num;
        slave_mask = slave_mask | temp;
        outb(slave_mask,SLAVE_8259_PORT);
        return;
    }
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
    int temp = EOI;
    if ((irq_num < 0) || (irq_num > 15)){return;} //invalid irq_number
    /*in master PIC*/
    if ((irq_num >= 0) && (irq_num <= 7))
    {
        temp = EOI + irq_num;
        outb(temp,MASTER_8259_PORT);
        return;
    }
    /*in slave PIC*/
    if ((irq_num >= 8) && (irq_num <= 15))
    {
        /*master port*/
        temp = EOI + 2;
        outb(temp,MASTER_8259_PORT);
        /*slave port*/
        temp = EOI + irq_num - 8;
        outb(temp,SLAVE_8259_PORT);
        return;
    }
}
