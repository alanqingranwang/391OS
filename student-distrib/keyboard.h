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

/*** remove later ***/
#include "testcases3_2.h"
#include "filesystem.h"
/********************/

/* Keyboard ports */
#define KBD_DATA_PORT   0x60
#define KBD_STATUS_PORT 0x64
#define KBD_IRQ 1

#define MAKE            0
#define BREAK           1
/* Scancode definitions */
#define ABC_LOW_SCANS 	0x02
#define ABC_HIGH_SCANS 	0x58
#define TOTAL_SCANCODES 128
#define GRAVE_SCAN_CODE	0x29

#define L_SHIFT_MAKE    0x2A
#define L_SHIFT_BREAK   0xAA
#define R_SHIFT_MAKE    0x36
#define R_SHIFT_BREAK   0xB6

#define CTRL_MAKE    0x1D
#define CTRL_BREAK   0x9D
#define L_MAKE       0x26

#define CAPS         0x3A
#define BKSP         0x0E
#define ENTER        0x1C
#define ALT 	 		0x38
#define FOUR_SCAN 	0x05
#define THREE_SCAN 	0x04
#define ONE_SCAN     0x02

#define fn1	0x3B
#define fn2 0x3C
#define fn3 0x3D

#define BUFFER_SIZE     128
#define KEY_MODES       4   // nothing, shift, caps, shift and caps
#define NUM_COLS        80

/*******************/
#define NONE_MODE 0
#define SHIFT_MODE 1
#define CAPS_MODE 2
#define SHIFT_CAPS_MODE 3

#define STDIN_FD 0 // the fd for STDIN

/* Externally-visible functions */

/* Initialize the Keyboard */
void keyboard_init();

/* Handle button presses */
void keyboard_handler();
void toggle_caps();
void toggle_shift(int type);
void toggle_ctrl(int type);
void toggle_alt(int type);
void process_key(uint8_t key);
void handle_backspace();
void handle_enter();
void clear_buffer();

int32_t keyboard_open(const uint8_t* blank1);
int32_t keyboard_read(int32_t fd, uint8_t* buf, int32_t nbytes);
int32_t keyboard_write(int32_t fd, const void* blank1, int32_t blank2);
int32_t keyboard_close(int32_t fd);

#endif /* _KEYBOARD_H */
