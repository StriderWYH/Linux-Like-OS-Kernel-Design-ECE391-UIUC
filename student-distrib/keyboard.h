#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "types.h"

#define KEYBOARD_IRQ    1
#define KEYBOARD_PORT   0x60
#define KEYBOARD_CMD   0x64


void keyboard_init();

void keyboard_interrupt_handler();


#endif

