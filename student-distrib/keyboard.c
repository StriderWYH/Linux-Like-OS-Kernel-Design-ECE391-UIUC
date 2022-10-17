#include "keyboard.h"
#include "lib.h"
#include "i8259.h"
int i = 0;
unsigned int pre = 0;

/*this is a scan_code table, which is used for search each key's ASCII by index*/
unsigned char scancode[58] = 
{
    0,0,'1','2','3','4','5','6','7','8','9','0','-','=',
    0,0,'q','w','e','r','t','y','u','i','o','p','[',']',
    '\n',0,'a','s','d','f','g','h','j','k','l',';',39,
    '`',0,'\\','z','x','c','v','b','n','m',',','.','/',
    0,0,0,'\0'
};

/*
 * introduction: initialize the keyboard
 * input: none
 * output: none
 */
void keyboard_init()
{
    enable_irq(KEYBOARD_IRQ);       //enable the keyboard to receive interrupt
}

/*
 * introduction: create a handler for idt to call 
 * input: none
 * output: none
 */
void keyboard_interrupt_handler()
{
    unsigned int key;
    unsigned int value;
    //printf("keyboard handler \n");  
    
    key = inb(KETBOARD_PORT) & 0xFF;    //low 8 bits 
    value = scancode[key];  //fond the ASCII
    //printf("%u",&value);
    if ((i%2 != 1) || (pre == value))       //ingnore the interrupt of releasing the button
    {
        putc(value);        //if not the interrupt of releasing buttons or contining pressing one button, allow to print
    }
    send_eoi(KEYBOARD_IRQ);
    i++;    //when i is even, it's the signal of pressing, when i is odd, it's the signal of releasing
    pre = value;
}
