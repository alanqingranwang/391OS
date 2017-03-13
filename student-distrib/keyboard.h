#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "i8259.h"
#include "idt.h"
#include "lib.h"

#define KBD_IRQ 1
#define KBD_DATA_PORT   0x60
#define KBD_STATUS_PORT 0x64


void keyboard_init();

void keyboard_handler();

#endif
