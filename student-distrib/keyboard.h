#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "types.h"

#define KEYBOARD_IRQ    1
#define KETBOARD_PORT   0x60


void keyboard_init();

void keyboard_interrupt_handler();


#endif

