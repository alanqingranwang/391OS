/* JC
 * rtc.c - Functions to interact with the 8259 PIC's RTC interrupt port
 * tab size = 3, no space
 */

#include "rtc.h"
#include "lib.h"







/* JC
 * rtc_init
 * 	DESCRIPTION:
 * 	INPUT: none
 *		OUTPUT:
 *		RETURN VALUE: none
 *		SIDE EFFECTS:
 *
 */
void rtc_init(void)
{
	uint32_t flags;
	int8_t prev_data;

	// Enable Periodic Interrupt, default 1024 Hz rate
	cli_and_save(flags);
	outb((DIS_NMI | REG_B), SELECT_REG); 	// select B and disable NMI
	prev_data = inb(CMOS_RTC_PORT);			// get current values of B
	outb((DIS_NMI | REG_B), SELECT_REG);	// set index again (a read resets the index to register D)
	outb((prev_data | BIT_6), 0x71);			// turn on bit 6 of reg B
	restore_flags(flags);

	// Register C needs to be read after an IRQ 8 otherwise IRQ won't happen again
	outb(REG_C, SELECT_REG);
	inb(CMOS_RTC_PORT);			// throw away data
}

/* JC
 * 
 * 	DESCRIPTION:
 * 	INPUT:
 *		OUTPUT:
 *		RETURN VALUE:
 *		SIDE EFFECTS:
 *
 */

