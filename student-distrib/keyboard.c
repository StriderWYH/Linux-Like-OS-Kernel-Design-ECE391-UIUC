#include "keyboard.h"
#include "lib.h"
#include "i8259.h"


//int global_keyboard_index = 0;
unsigned int status_shift = 0;  //0 for release, 1 for press
unsigned int status_ctrl = 0;   //0 for release, 1 for press
unsigned int status_caps = 0;   //0 for unlock, 1 for lock
unsigned int special_change = 0;    //0 for no special button change
/*this is a scan_code table, which is used for search each key's ASCII by global_keyboard_index*/
volatile int keyboard_flag = 0;


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
    8,0,'Q','W','E','R','T','Y','U','I','O','P','{','}',
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
    8,0,'q','w','e','r','t','y','u','i','o','p','{','}',
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
    special_change = 0;
    if ((key == LSHIFT_PRESS) || (key == RSHIFT_PRESS))
    {
        status_shift = 1;
        special_change = 1;
    }
    if ((key == LSHIFT_RELEASE) || (key == RSHIFT_RELEASE))
    {
        status_shift = 0;
        special_change = 1;
    }
    if (key == CAPS)
    {
        status_caps = ~status_caps;
        status_caps = status_caps & 0X01;
        special_change = 1;
    }
    if (key == CTRL_PRESS)
    {
        status_ctrl = 1;
        special_change = 1;
    }
    if (key == CTRL_RELEASE)
    {
        status_ctrl = 0;
        special_change = 1;
    }
    return;
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
    key = 0;
    key = inb(KEYBOARD_PORT) & 0xFF;    //low 8 bits

    special_button_status(key);

    if (key == 0x39)    //0x39 is the scancode for space
    {
        if (global_keyboard_index < 127)
        {
            keyboard_buffer[global_keyboard_index++] = 32;  //32 for blank
            print_stuff(32,global_keyboard_index);
        }
        send_eoi(KEYBOARD_IRQ);
        return;
    }
    if (key == 0x0F)    //0x0F is the scancode for TAB
    {
        if (global_keyboard_index <= 127-4)
        {
            keyboard_buffer[global_keyboard_index++] = 32;  //32 for blank
            print_stuff(32,global_keyboard_index);
            keyboard_buffer[global_keyboard_index++] = 32;  //32 for blank
            print_stuff(32,global_keyboard_index);
            keyboard_buffer[global_keyboard_index++] = 32;  //32 for blank
            print_stuff(32,global_keyboard_index);
            keyboard_buffer[global_keyboard_index++] = 32;  //32 for blank
            print_stuff(32,global_keyboard_index);
        }
        send_eoi(KEYBOARD_IRQ);
        return;
    }

    if (key>=0 && key<=57)
    {
        if ((status_ctrl==1) && (key == 0x26))  //scancode of 'l' is 0x26
        {
            clean_screen();
            send_eoi(KEYBOARD_IRQ);
            return;
        }
        if (key == ENTER)
        {
            //terminal_read(global_keyboard_index);
            //terminal_write(terminal_read(global_keyboard_index));
            // int write;
            // write = terminal_read(global_keyboard_index);
            // terminal_write(write);
    		// terminal_read();

            keyboard_flag = 1;
            send_eoi(KEYBOARD_IRQ);
            return;
        }
        if (key == BACKSPACE)
        {
            //int old_index = global_keyboard_index;
            if (global_keyboard_index > 0){
                keyboard_buffer[global_keyboard_index-1] = '\0';
                global_keyboard_index = global_keyboard_index -1;
                change_line(-1);
                update_cursor(0);
            }
            else{
                send_eoi(KEYBOARD_IRQ);
                return; 
            }
            ///////////////////////////
            send_eoi(KEYBOARD_IRQ);
            return;
            ///////////////////////////
        }
        if (special_change == 1)
        {
            send_eoi(KEYBOARD_IRQ);
            return;
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
            send_eoi(KEYBOARD_IRQ);
            return;
        }
        
        if (global_keyboard_index < 127)
        {
            keyboard_buffer[global_keyboard_index++] = value;
            print_stuff(value,global_keyboard_index);
            //putc(value);
        }
    } 
        
    send_eoi(KEYBOARD_IRQ);
    return;
} 
/* print_stuff
 * introduction: a modified version of putc, which can change line and scroll down according to the index
 * input: value: the value to print; indexP, the index of it on the screen, which is almost the same as screen_X
 * output: none
 */
void print_stuff(int value,int indexP){
    if((indexP % 80) == 0){ // check whether the buffer has filled a row, 80 is the length of the x-axis
        putc(value);
        change_line(1);  // change line by one, if necssary scroll down for one line
        update_cursor(0);  // the offset is not sure
    }
    else{
        putc(value);
        update_cursor(0);
    }
    return;
}

/* terminal_open
 * introduction: clean the screen and update the cursor, which should be added more function in later checkpoints
 * input: nbytes : the bytes to be read
 * output: 0 if success
 */
//////////////////////////////////////////////////////
int terminal_open(int nbytes){
    clean_screen();                     // clean the screen first
    update_cursor(0);                   // update the cursor
    return 0;
}
/* terminal_close
 * introduction: close the terminal
 * input: nbytes
 * output: 0 if success
 */
int terminal_close(int nbytes){
    disable_cursor();
    return 0;
}
/* terminal_read
 * introduction: read the character from the keyboard and store it in the terminal buffer
 * input: nbytes: bytes to be read
 * output: bytes that is successfully read
 */
int terminal_read(int nbytes){
    int byte_read;
    int i = 0;
    byte_read = 0;
    while(!keyboard_flag);              // wait untill the keyboard has entered a "enter"
    keyboard_flag = 0;                  // reset the flag to be used next time
    /////////////////// Maybe space for enter actions
    //putc((int)('\n')); // change the line
    if((global_keyboard_index) != 80){  // if the index is 80, that should change one line
        change_line(1);
    }
   

    update_cursor(0);


    /////////////////// Maybe space for enter actions

    for(i = 0; i < global_keyboard_index; i++){
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
        terminal_buffer[i] = keyboard_buffer[i];
        byte_read += 1;
        keyboard_buffer[i] = '\0'; // clear the buffer
    }
    terminal_buffer[global_keyboard_index] = '\n';
    byte_read++;
    //printf("%d",&nbytes);
    global_keyboard_index = 0;
    return byte_read;
}
/* terminal_write 
 * introduction: write the bytes read into the buffer
 * input: nbytes : bytes to be written
 * output: bytes that are successfully written
 */
int terminal_write(int nbytes){
    int byte_write;
    int i = 0;
    byte_write = 0;
    //if (nbytes > 80)  scrolling(1);

    for(i = 0; i < nbytes; i++){
        if((i == 80) & (terminal_buffer[i] != '\n')){    // if it meet the end of the line or user pressed an "enter"
            change_line(1);
        }
        if(terminal_buffer[i] == '\n'){
            change_line(1);
        }
        else{
            putc(terminal_buffer[i]);
        }

        byte_write += 1;
        terminal_buffer[i] = '\0'; // clear the buffer
        update_cursor(0); // update the cursor by one place 
    
    }
    return byte_write;
}
