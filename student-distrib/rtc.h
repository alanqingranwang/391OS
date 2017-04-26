/* JC
 * rtc.h - Declarations used in interactions with the RTC interrupt port
 *		on the PIC. IRQ 8 (IRQ 0 on the slave)
 *	tab size = 3, no space
 */

/* Details about the RTC:
 *		http://wiki.osdev.org/RTC
 *		RTC is capable of multiple frequencies but is pre-programmed
 *			at 32.768 kHz. This is the only one that keeps proper time.
 *		Strongly advised not to change this base frequency.
 *
 *		The output (interrupt) divider frequency is by default set so
 *			that there is an interrupt rate of 1024 Hz.
 *
 *		RTC interrupts are disabled by default. If you turn on the RTC
 *			interrupts, the RTC will periodically generate IRQ 8.
 */

#ifndef _RTC_H
#define _RTC_H

#include "i8259.h"
#include "lib.h"
/* adding the interrupt to the table is the job of the init */
#include "idt.h"


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

#define RTC_IRQ			8		// slave IRQ 0

/* Write these to port 0x70 to select and/or disable NMI.
 *	Register A - used to select an interrupt rate.
 *		(freq = 32768 >> (rate-1)) calculates interrupt rate
 *	Register B - contains flags, bit 6 (Periodic Interrupt Enable)
 *		Used in time also, tells what format the time will come in
 *	Register C - holds bit mask that tells what kind of interrupt occurred
 *		must read from this otherwise IRQ 8 will not generate again.
 */
#define REG_A				0x0A	// register A
#define REG_B				0x0B	// register B
#define REG_C				0x0C	// register C
#define DISABLE_NMI		0x80	// disable NMI bit

/* time format bits of register B */
#define PERIODIC			0x40 	// enables periodic interrupt
#define HOUR_BIT			0x02	// enables 24 hour format if set
#define BINARY_MODE_BIT 0x04 	// enables binary mode if set

/* Keeping track of time */
#define CURRENT_YEAR		2017

/* Registers for each time info */
#define SEC_REG 			0x00
#define MIN_REG			0x02
#define HOUR_REG			0x04
#define DAY_REG			0x07
#define MONTH_REG 		0x08
#define YEAR_REG 			0x09

#define NIBBLE_MASK 		0x0F
#define CENTURY			100
#define TENS 				10
#define SHIFT4				16

#define DEFAULT_FREQ		2 	// 2Hz = 2 interrupts/second
#define MAX_RATE			6		// 1024Hz
#define MIN_RATE			15
#define NUM_FREQ			14

/* Externally-visible functions */

/* Initialize the RTC */
void rtc_init(void);
void rtc_handler(void);

/* Real-Time Clock Driver */
int32_t rtc_open(const uint8_t* blank1);
int32_t rtc_read(int32_t fd, uint8_t* blank1, int32_t blank2);
int32_t rtc_write(int32_t fd, const void* buf, int32_t blank1);
int32_t rtc_close(int32_t fd);

void set_frequency(uint32_t frequency);

/* Additional Functionalities */
/* Following 4 functions are used to
 * update the static global time variables
 */
int32_t get_update_flag(void);
uint8_t get_RTC_reg(int32_t reg);
void update_time(void);
void binary_to_real_time(void);

/* Calls binary_to_real_time to get current
 * time and prints them out
 */
void print_time(void);

#endif /* _RTC_H */
