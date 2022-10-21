#include "keyboard.h"
#include "lib.h"
#include "i8259.h"

char keyboard_buffer[BUFFERSIZE];
char buffer[BUFFERSIZE];
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
int terminal_open(char* buffer, int nbytes){
    update_cursor(0);
    return 0;
}

int terminal_close(char* buffer, int nbytes){
    return 0;
}

int terminal_read(char* buffer, int nbytes){
    int byte_read = 0;
    int i = 0;


    /////////////////// Maybe space for enter actions
    //putc((int)('\n')); // change the line
    change_line();
    update_cursor(0);


    /////////////////// Maybe space for enter actions

    for(i = 0; i < nbytes; i++){
        /*
        if(keyboard_buffer[i] == '\n'){ // when the enter is detected, end reading
            keyboard_buffer[i] = '\0';
            break;
        }
        */
        if(i >= 128){    // check for overflow
            keyboard_buffer[i] = '\0';
            break;
        }
        buffer[i] = keyboard_buffer[i];
        byte_read += 1;
        keyboard_buffer[i] = '\0'; // clear the buffer
    }
    return byte_read;
}

int terminal_write(char* buffer, int nbytes){
    int byte_write = 0;
    int i = 0;


    for(i = 0; i < nbytes; i++){
        if(buffer[i] == '\0'){
            break;
        }
        putc(buffer[i]);
        byte_write += 1;
        buffer[i] = '\0'; // clear the buffer
        update_cursor(1); // update the cursor by one place        
    }
    return byte_write;
}
