#include "keyboard.h"
#include "lib.h"
#include "i8259.h"

char keyboard_buffer[BUFFERSIZE];
char buffer[BUFFERSIZE];
int index = 0;
unsigned int status_shift = 0;  //0 for release, 1 for press
unsigned int status_ctrl = 0;   //0 for release, 1 for press
unsigned int status_caps = 0;   //0 for unlock, 1 for lock
/*this is a scan_code table, which is used for search each key's ASCII by index*/


//int keyboard_length = 0;
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
        if (key == ENTER)
        {
            //terminal_read(index);
            terminal_write(terminal_read(index));
        }
        if (key == BACKSPACE)
        {
            int old_index = index;
            keyboard_buffer[index--] = '\0';
            if ((index <= 79) && (old_index >= 80))
            {
                change_line(-1);
            }
            update_cursor(0);
            ///////////////////////////

            ///////////////////////////
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
            print_stuff(value);
            //putc(value);
        }
        


    } 
        
    send_eoi(KEYBOARD_IRQ);
}

void print_stuff(value){
    if(index <= 80 - 1){ // check whether the buffer has filled a row
        putc(value);
        update_cursor(1);
    }
    else{
        change_line(1);
        putc(value);
        update_cursor(0);  // the offset is not sure
    }
}


//////////////////////////////////////////////////////
int terminal_open(char* buffer, int nbytes){
    update_cursor(0);
    return 0;
}

int terminal_close(char* buffer, int nbytes){
    return 0;
}

int terminal_read(int nbytes){
    int byte_read = 0;
    int i = 0;


    /////////////////// Maybe space for enter actions
    //putc((int)('\n')); // change the line
    change_line(1);
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
    buffer[nbytes] = '\n';
    index = 0;
    return byte_read;
}

int terminal_write(int nbytes){
    int byte_write = 0;
    int i = 0;


    for(i = 0; i < nbytes; i++){
        if(buffer[i] == '\n'){
            break;
        }
        putc(buffer[i]);
        byte_write += 1;
        buffer[i] = '\0'; // clear the buffer
        update_cursor(1); // update the cursor by one place        
    }
    return byte_write;
}
