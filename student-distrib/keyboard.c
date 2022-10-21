#include "keyboard.h"
#include "lib.h"
#include "i8259.h"

char keyboard_buffer[128];
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
    
    key = inb(KEYBOARD_PORT) & 0xFF;    //low 8 bits
    if (key>=0 && key<=57)
    {
        value = scancode[key];  //fond the ASCII
        putc(value);
    }
    
    
    
    
        
    send_eoi(KEYBOARD_IRQ);
}












//////////////////////////////////////////////////////
int terminal_open(){

}

int terminal_close(){
    
}

int terminal_read(){
    
}

int terminal_write(){
    
}
