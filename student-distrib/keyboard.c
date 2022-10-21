#include "keyboard.h"
#include "lib.h"
#include "i8259.h"

char keyboard_buffer[128];
int index = 0;
unsigned int status_shift = 0;  //0 for release, 1 for press
unsigned int status_ctrl = 0;   //0 for release, 1 for press
unsigned int status_caps = 0;   //0 for unlock, 1 for lock
/*this is a scan_code table, which is used for search each key's ASCII by index*/
unsigned char scancode_lower[58] = 
{
    0,0,'1','2','3','4','5','6','7','8','9','0','-','=',
    8,0,'q','w','e','r','t','y','u','i','o','p','[',']',
    '\n',0,'a','s','d','f','g','h','j','k','l',';',39,
    '`',0,'\\','z','x','c','v','b','n','m',',','.','/',
    0,0,0,'\0'
};
unsigned char scancode_upper[58] =
{
    0,0,'!','@','#','$','%','^','&','*','(',')','_','+',
    '8','0','Q','W','E','R','T','Y','U','I','O','P','{','}',
    '\n',0,'A','S','D','F','G','H','J','K','L',':',34,
    '~',0,'|','Z','X','C','V','B','N','M','<','>','?',
    0,0,0,'\0'
};
unsigned char scancode_caps_only[58] = 
{
    0,0,'1','2','3','4','5','6','7','8','9','0','-','=',
    8,0,'Q','W','E','R','T','Y','U','I','O','P','[',']',
    '\n',0,'A','S','D','F','G','H','J','K','L',';',39,
    '`',0,'\\','Z','X','C','V','B','N','M',',','.','/',
    0,0,0,'\0'
};
unsigned char scancode_shift_caps[58] = 
{
    0,0,'!','@','#','$','%','^','&','*','(',')','_','+',
    '8','0','q','w','e','r','t','y','u','i','o','p','{','}',
    '\n',0,'a','s','d','f','g','h','j','k','l',':',34,
    '~',0,'|','z','x','c','v','b','n','m','<','>','?',
    0,0,0,'\0'
};

/*
 * 
 * 
 */
void special_button_status(unsigned int key)
{
    if ((key == LSHIFT_PRESS) || (key == RSHIFT_PRESS))
    {
        status_shift = 1;
    }
    if ((key == LSHIFT_RELEASE) || (key == RSHIFT_RELEASE))
    {
        status_shift = 0;
    }
    if (key == CAPS)
    {
        status_caps = ~status_caps;
    }
    if (key == CTRL_PRESS)
    {
        status_ctrl = 1;
    }
    if (key == CTRL_RELEASE)
    {
        status_ctrl = 0;
    }
}



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
    special_button_status(key);
    if (key>=0 && key<=57)
    {
        if ((status_ctrl==1)&&(key = 'l'))
        {
            clean_screen();
        }
        if ((status_caps==0)&&(status_shift==0))
        {
            value = scancode_lower[key];
        }
        if ((status_caps==1)&&(status_shift==0))
        {
            value = scancode_caps_only[key];
        }
        if ((status_caps==0)&&(status_shift==1))
        {
            value = scancode_upper[key];
        }
        if ((status_caps==1)&&(status_shift==1))
        {
            value = scancode_shift_caps[key];
        }
        
        if (value == '\0')
        {
            return;
        }
        
        if (index <= 127)
        {
            keyboard_buffer[index++] = value;
            putc(value);
        }
        


    }
    
    
    
    
        
    send_eoi(KEYBOARD_IRQ);
}












//////////////////////////////////////////////////////
int terminal_open(char* buffer, int nbytes){
    return 0;
}

int terminal_close(char* buffer, int nbytes){
    return 0;
}

int terminal_read(char* buffer, int nbytes){
    
}

int terminal_write(char* buffer, int nbytes){
    
}
