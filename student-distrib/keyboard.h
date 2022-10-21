#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "types.h"

#define KEYBOARD_IRQ    1
#define KEYBOARD_PORT   0x60
#define KEYBOARD_CMD   0x64

#define BUFFERSIZE 128

void keyboard_init();

void keyboard_interrupt_handler();

int terminal_open(char* buffer, int nbytes);
int terminal_close(char* buffer, int nbytes);
int terminal_read(char* buffer, int nbytes);
int terminal_write(char* buffer, int nbytes);
#endif

