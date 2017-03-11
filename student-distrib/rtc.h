/* JC
 * rtc.h - Defines used in interactions with the RTC interrupt port
 *		on the PIC. IRQ 8 (IRQ 0 on the slave)
 *	tab size = 3, no space
 */

/* Details about the RTC:
 *		http://wiki.osdev.org/RTC
 *		RTC is capable of multiple frequencies but is pre-programmed
 *			at 32.768 kHz. This is the only one that keeps proper time.
 *		Stronly advised not to change this base frequency.
 *		
 *		The output (interrupt) divider frequency is by default set so
 *			that there is an interrupt rate of 1024 Hz.
 *
 *		RTC interrupts are disabled by default. If you turn on the RTC
 *			interrupts, the RTC will periodically generate IRQ 8.
 */

#ifndef _RTC_H
#define _RTC_H

#include "types.h"
//#include "i8259.h" // do I need to include this?

/* port 0x70 is used to specify an index or "register number"
 *		and to disable non-maskable-interrupt (NMI).
 *		High order bit tells the hardware enable/disable NMIs
 *		bit = 1, disabled until next time byte is sent
 *		Low order of any byte is used to address CMOS registers.
 * port 0x71 is used to read or write from/to that byte of CMOS
 *		configuration space.
 * 
 *	Only three bytes are used, which are called RTC Status Register A, B, and C.
 * 	Offset 0xA, 0xB, 0xC in CMOS RAM.
 */
#define SELECT_REG		0x70	// output to this port to select register
#define CMOS_RTC_PORT	0x71	// r/w from/to the CMOS configuration space

/* Write these to port 0x70 to select and/or disable NMI.
 */
#define REG_A				0x0A	// register A
#define REG_B				0x0B	// register B
#define REG_C				0x0C	// register C
#define DIS_NMI			0x80	// disable NMI bit
#define BIT_6				0x40 	// 0010 0000


/* Externally-visible functions */

/* Initialize the RTC */
void rtc_init(void);



#endif /* _RTC_H */

