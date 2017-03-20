/* JC
 * keyboard.h - Defines used in interactions with the keyboard interrupt
 *		on the PIC IRQ 1.
 *	tab size = 3, no space
 */

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "i8259.h"
#include "idt.h"
#include "lib.h"

#define KBD_IRQ 0x21
#define KBD_DATA_PORT   0x60
#define KBD_STATUS_PORT 0x64

static unsigned char keyboard_map[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};

void keyboard_init();

void keyboard__handler();


#include "lib.h"
/* adding the interrupt to the table is the job of the init */
#include "idt.h"

/* Keyboard ports */
#define KBD_DATA_PORT   0x60
#define KBD_STATUS_PORT 0x64

#define KBD_IRQ 1					// the PIC IRQ for keyboard

/* Scancode definitions */
#define ABC_LOW_SCANS 	0x02
#define ABC_HIGH_SCANS 	0x32
#define TOTAL_SCANCODES 36
#define GRAVE_SCAN_CODE	0x29

/* Externally-visible functions */

/* Initialize the Keyboard */
void keyboard_init();
void keyboard_handler();

#endif /* _KEYBOARD_H */
