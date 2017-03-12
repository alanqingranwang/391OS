/* JC
 * rtc.c - Functions to interact with the 8259 PIC's RTC interrupt port
 * tab size = 3, no space
 */

#include "rtc.h"
#include "lib.h"

/* Keeps track of current time */
static uint8_t second;
static uint8_t minute;
static uint8_t hour;
static uint8_t day;
static uint8_t month;
static uint32_t year;

/* JC
 * rtc_init
 * 	DESCRIPTION:
 *			Turns on periodic interrupt from RTC.
 *			Enables IRQ 8 so that the PIC will accept the interrupts
 * 	INPUT: none
 *		OUTPUT: none
 *		RETURN VALUE: none
 *		SIDE EFFECTS: Enables Periodic RTC IRQ 8
 *		NOTE:
 *			Don't have a huge separate between a register select instruction and a r/w instructions
 *				This will cause complications to the chip.
 */
void rtc_init(void)
{
	uint32_t flags;
	int8_t prev_data;

	// Enable Periodic Interrupt, default 1024 Hz rate
	cli_and_save(flags);
	outb((DISABLE_NMI | REG_B), SELECT_REG); 	// select B and disable NMI
	prev_data = inb(CMOS_RTC_PORT);				// get current values of B
	outb((DISABLE_NMI | REG_B), SELECT_REG);	// set index again (a read resets the index to register D)
	outb((prev_data | PERIODIC), CMOS_RTC_PORT);		// turn on bit 6 of reg B
	enable_irq(RTC_IRQ);								// enable PIC to accept interrupts
	restore_flags(flags);
}

/* JC
 * get_update_flag
 * 	DESCRIPTION:
 *			reads from register A, and checks whether NMI is enable/disabled
 * 	INPUT: none
 *		OUTPUT: none
 *		RETURN VALUE: 0 - disabled
 *						  0x80 - enabled
 *		SIDE EFFECTS: none
 *
 */
int32_t get_update_flag(void)
{
	outb(REG_A, SELECT_REG);
	return inb(CMOS_RTC_PORT) & DISABLE_NMI; // check if NMI disabled
}

/* JC
 * get_RTC_reg
 * 	DESCRIPTION:
 *			Takes a given register and access the register, then returns
 *				the information in the register
 * 	INPUT: register
 *		OUTPUT: none
 *		RETURN VALUE: uint8_t - data from given register
 *		SIDE EFFECTS: none
 *
 */
uint8_t get_RTC_reg(int32_t reg)
{
	outb(reg, SELECT_REG);
	return inb(CMOS_RTC_PORT); // read from it
}

/* JC
 * update_time
 * 	DESCRIPTION:
 *			Updates all the local variables declared at the top.
 * 	INPUT: none
 *		OUTPUT: none
 *		RETURN VALUE: none
 *		SIDE EFFECTS: none
 *
 */
void update_time(void)
{
	second = get_RTC_reg(SEC_REG);
	minute = get_RTC_reg(MIN_REG);
	hour = get_RTC_reg(HOUR_REG);
	day = get_RTC_reg(DAY_REG);
	month = get_RTC_reg(MONTH_REG);
	year = get_RTC_reg(YEAR_REG);
}

/* JC
 * read_time
 * 	DESCRIPTION:
 *			Reads all the required info registers and converts
 *				it to current time.
 * 	INPUT: none
 *		OUTPUT: none
 *		RETURN VALUE: none
 *		SIDE EFFECTS: none
 *
 */
void read_time(void)
{
	// hold the previous data
	uint8_t last_second;
	uint8_t last_minute;
	uint8_t last_hour;
	uint8_t last_day;
	uint8_t last_month;
	uint8_t last_year;
	uint8_t registerB; // holds data for register B

	// wait till we can interrupt
	while(get_update_flag()); 
	update_time(); // update time variables

	// keep getting new time till it's different
	while((last_second != second) || (last_minute != minute) ||
		(last_hour != hour) || (last_day != day) || (last_month != month) ||
		(last_year != year))
	{	// get the new time
		last_second = second;
		last_minute = minute;
		last_hour = hour;
		last_day = day;
		last_month = month;
		last_year = year;

		while(get_update_flag());
		update_time(); // update time variables
	}

	registerB = get_RTC_reg(REG_B); // read data

	if(!(registerB & BINARY_MODE_BIT))
	{	// convert to proper time
		second = (second & NIBBLE_MASK) + ((second/SHIFT4) * TENS);
		minute = (minute & NIBBLE_MASK) + ((minute/SHIFT4) * TENS);
		hour = (((hour & NIBBLE_MASK) + (((hour & 0x70)/SHIFT4) * TENS)) | (hour & 0x80));
		day = (day & NIBBLE_MASK) + ((day/SHIFT4) * TENS);
		month = (month & NIBBLE_MASK) + ((month/SHIFT4) * TENS);
		year = (year & NIBBLE_MASK) + ((year/SHIFT4) * TENS);
	}

	// convert from 12 hour to 24 hour if necessary
	if(!(registerB & HOUR_BIT) && (hour & 0x80))
		hour = ((hour & 0x7F) + 12) % 24;

	// calculate full year
	year += (CURRENT_YEAR/CENTURY) * CENTURY;
	if(year < CURRENT_YEAR)
		year += CENTURY;
}

/* JC - Used as a test for interrupt
 * print_time
 * 	DESCRIPTION:
 *			Reads from the local variables and prints out the time
 *			This is used for IRQ 8 test
 * 	INPUT: none
 *		OUTPUT: outputs time
 *		RETURN VALUE: none
 *		SIDE EFFECTS: none
 *
 */
void print_time(void)
{
	read_time();
	printf("second: %d ", second);
	printf("minute: %d ", minute);
	printf("hour: %d ", hour);
	printf("day: %d ", day);
	printf("month: %d ", month);
	printf("year: %d\n", year);

	// Register C needs to be read after an IRQ 8 otherwise IRQ won't happen again
	outb(REG_C, SELECT_REG);
	inb(CMOS_RTC_PORT);			// throw away data

	send_eoi(RTC_IRQ);
}

/* JC
 * rtc_handler
 * 	DESCRIPTION:
 *			
 * 	INPUT: none
 *		OUTPUT: none
 *		RETURN VALUE: 
 *		SIDE EFFECTS: none
 *
 */
void rtc_handler(void)
{
	// save registers
	//save_registers();
	asm volatile("pushal");
	printf("a");
	// Register C needs to be read after an IRQ 8 otherwise IRQ won't happen again
	outb(REG_C, SELECT_REG);
	inb(CMOS_RTC_PORT);			// throw away data

	send_eoi(RTC_IRQ);
	asm volatile("popal \n 	\
		iret");	// consumes the stack, doesn't tear down stack
	//restore_registers();
}
