#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "types.h"

#define KEYBOARD_IRQ    1
#define KETBOARD_PORT   0x60


void keyboard_init();

void keyboard_interrupt_handler();



unsigned char scancode[58] = 
{
    0 ,'`' , '1' , '2' , '3' , '4' , '5' , '6' , '7' , '8' , '9' , '0' , '-' , '=' , 8 , 0 ,
    'q' , 'w' , 'e' , 'r' , 't' , 'y' , 'u' , 'i' , 'o' , 'p' , '[' , ']' , '\\' , 0 , 'a' , 's' ,
    'd' , 'f' , 'g' , 'h' , 'j' , 'k' , 'l' , ';' , 39 , '\n' , 0 , 'z' , 'x' , 'c' , 'v' , 'b' ,
    'n' , 'm' , ',' , '.' , '/' , 0 , 0 , 0 , 0 , 0
};

#endif