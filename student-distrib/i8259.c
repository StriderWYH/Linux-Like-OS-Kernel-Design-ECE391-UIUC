/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask = 0xFF; /* IRQs 0-7  */
uint8_t slave_mask = 0xFF;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */
/*
 * introduction: initialize the PIC
 * input: none
 * output: none
 */
void i8259_init(void) {
    outb(0xFF,0x21);    
    outb(0xFF,0xA1);    //stop all the interrupt
    
    outb(ICW1,MASTER_8259_PORT); //ICW1 master
    outb(ICW1,SLAVE_8259_PORT); //ICW1 slave
    outb(ICW2_MASTER,0x21); //0x21 master_8259_port + 1
    outb(ICW2_SLAVE,0xA1);  //0xA1 slave_8259_port + 1
    outb(ICW3_MASTER,0x21); //0x21 master_8259_port + 1
    outb(ICW3_SLAVE,0xA1);  //0xA1 slave_8259_port + 1
    outb(ICW4,0x21); //0x21 master_8259_port + 1
    outb(ICW4,0xA1);  //0xA1 slave_8259_port + 1

    outb(master_mask,0x21); //stop init
    outb(slave_mask,0xA1);
    enable_irq(0x02);
}

/* Enable (unmask) the specified IRQ */
/*
 * introduction: enable the IRQ
 * input: irq_num
 * output: none
 */
void enable_irq(uint32_t irq_num) {
    int temp = 0x01;
    //int IRQ2_not = 0x04;    //0000 0100
    if ((irq_num < 0) || (irq_num > 15)){return;} //invalid irq_number
    /*in master PIC*/
    if ((irq_num >= 0) && (irq_num <= 7))   //master is from 0~7
    {
        temp = 1 << irq_num;
        temp = ~temp;       //if in master PIC, change the coresponding bit to 0 on the master
        master_mask = master_mask & temp;
        outb(master_mask,MASTER_8259_PORT+1);
        return;
    }
    /*in slave PIC*/
    if ((irq_num >= 8) && (irq_num <= 15))      //slave is from 8~15
    {
        /*master port*/
        // master_mask = master_mask & (~IRQ2_not);
        // outb(master_mask,MASTER_8259_PORT+1);
        /*slave port*/
        irq_num = irq_num - 8;  //irq_number in slave, master from 0~7
        temp = 1 << irq_num;
        temp = ~temp;       //if in slave PIC, change the coresponding bit to 0 on the slave
        slave_mask = slave_mask & temp;
        outb(slave_mask,SLAVE_8259_PORT+1);
        return;
    }
}


/* Disable (mask) the specified IRQ */
/*
 * introduction: disable the IRQ
 * input: irq_num
 * output: none
 */
void disable_irq(uint32_t irq_num) {
    int temp = 0x01;
    //int IRQ2 = 0x04;
    if ((irq_num < 0) || (irq_num > 15)){return;} //invalid irq_number
    /*in master PIC*/
    if ((irq_num >= 0) && (irq_num <= 7))   //master is from 0~7
    {
        temp = temp << irq_num;
        master_mask = master_mask | temp;       //if in master PIC, change the coresponding bit to 1 on the master
        outb(master_mask,MASTER_8259_PORT+1);
        return;
    }
    /*in slave PIC*/
    if ((irq_num >= 8) && (irq_num <= 15))      //slave is from 8~15
    {
        /*master port*/
        // master_mask = master_mask | IRQ2;
        // outb(master_mask,MASTER_8259_PORT+1);
        /*slave port*/
        irq_num = irq_num - 8;  //irq_number in slave, master from 0~7
        temp = temp << irq_num;
        slave_mask = slave_mask | temp;     //if in slave PIC, change the coresponding bit to 1 on the slave
        outb(slave_mask,SLAVE_8259_PORT+1);
        return;
    }
}

/* Send end-of-interrupt signal for the specified IRQ */
/*
 * introduction: send the EOI signal
 * input: irq_num
 * output: none
 */
void send_eoi(uint32_t irq_num) {
    int temp = EOI;
    if ((irq_num < 0) || (irq_num > 15)){return;} //invalid irq_number
    /*in master PIC*/
    if ((irq_num >= 0) && (irq_num <= 7))   //master is from 0~7
    {
        temp = EOI + irq_num;
        outb(temp,MASTER_8259_PORT);        //because the low 4 bits of EOI are 0s, and high 4 bits of IRQ_mun are 0s
        return;
    }
    /*in slave PIC*/
    if ((irq_num >= 8) && (irq_num <= 15))  //slave is from 8~15
    {
        /*master port*/
        temp = EOI + 2;         //slave connect to master through IRQ2
        outb(temp,MASTER_8259_PORT);
        /*slave port*/
        temp = EOI + irq_num - 8;       //master have 8 IRQ in total
        outb(temp,SLAVE_8259_PORT);
        return;
    }
}
