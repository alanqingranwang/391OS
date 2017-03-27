/* JC
 * keyboard.h - Defines used in interactions with the keyboard interrupt
 *		on the PIC IRQ 1.
 *	tab size = 3, no space
 */

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "i8259.h"
#include "lib.h"

/* adding the interrupt to the table is the job of the init */
#include "idt.h"

/* Keyboard ports */
#define KBD_DATA_PORT   0x60
#define KBD_STATUS_PORT 0x64
#define KBD_IRQ 1

#define MAKE            0
#define BREAK           1
/* Scancode definitions */
#define ABC_LOW_SCANS 	0x02
#define ABC_HIGH_SCANS 	0x39
#define TOTAL_SCANCODES 128
#define GRAVE_SCAN_CODE	0x29

#define L_SHIFT_MAKE    0x2A
#define L_SHIFT_BREAK   0xAA
#define R_SHIFT_MAKE    0x36
#define R_SHIFT_BREAK   0xB6

#define CTRL_MAKE    0x1D
#define CTRL_BREAK   0x9D
#define L_MAKE       0x26

#define CAPS            0x3A
#define BKSP            0x0E
#define ENTER           0x1C

static unsigned char buffer[128];
/* Externally-visible functions */

/* Initialize the Keyboard */
void keyboard_init();
void keyboard_handler();
void toggle_caps();
void toggle_shift(int type);
void toggle_ctrl(int type);
void process_key(uint8_t key);
void handle_backspace();
void handle_enter();
void clear_buffer();

#endif /* _KEYBOARD_H */
