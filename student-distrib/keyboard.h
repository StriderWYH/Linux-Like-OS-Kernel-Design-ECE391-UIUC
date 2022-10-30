#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "types.h"

#define KEYBOARD_IRQ    1
#define KEYBOARD_PORT   0x60
#define KEYBOARD_CMD   0x64

#define BUFFERSIZE 128
#define TERMINAL_SIZE 1000

#define TAB     0x0F
#define CAPS    0x3A
#define LSHIFT_PRESS    0x2A
#define LSHIFT_RELEASE  0xAA
#define RSHIFT_PRESS    0x36
#define RSHIFT_RELEASE  0xB6
#define CTRL_PRESS      0x1D
#define CTRL_RELEASE    0x9D
#define BACKSPACE       0x0E
#define ENTER           0x1C

int global_keyboard_index;
char keyboard_buffer[BUFFERSIZE];
char terminal_buffer[TERMINAL_SIZE];
volatile int keyboard_flag;

void special_button_status(unsigned int key);

void keyboard_init();

void keyboard_interrupt_handler();

void print_stuff(int value, int indexP);

int terminal_open(int nbytes);
int terminal_close(int nbytes);
int terminal_read(int32_t fd, void* buf, int32_t nbytes);
int terminal_write(int32_t fd, void* buf, int32_t nbytes);
#endif

