/* JC
 * keyboard.h - Defines used in interactions with the keyboard interrupt
 *		on the PIC IRQ 1.
 *	tab size = 3, no space
 */

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "i8259.h"
#include "lib.h"

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
