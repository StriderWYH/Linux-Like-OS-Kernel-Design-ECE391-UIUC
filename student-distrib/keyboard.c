#include "keyboard.h"
#include "lib.h"
#include "i8259.h"


void keyboard_init()
{
    enable_irq(KEYBOARD_IRQ);
}

void keyboard_interrupt_handler()
{
    unsigned int key;
    unsigned int value;
    send_eoi(KEYBOARD_IRQ);
    key = inb(KETBOARD_PORT) & 0xFF;    //low 8 bits 
    value = scancode[key];
    putc(value);
}