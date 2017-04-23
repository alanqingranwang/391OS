/* JC
 * rtc.c - Functions to interact with the 8259 PIC's RTC interrupt port
 * tab size = 3, no space
 */
#include "rtc.h"
#include "fd_table.h"

/* Interrupt Happened Flag */
static volatile uint32_t interrupt_flag; // When interrupt happens this changes to 1
// int32_t rtc_fd; // holds the rtc's fd when opened
/* Keeps track of current time */
static uint8_t second;
static uint8_t minute;
static uint8_t hour;
static uint8_t day;
static uint8_t month;
static uint32_t year;
static uint8_t curr_rate;
static uint32_t curr_frequency;
/* Purposely didn't include 2 Hz, it will be a default if an invalid freq is selected */
static uint32_t frequencies[NUM_FREQ] = {32768, 16384, 8192, 4096, 2048, 1024,
				512, 256, 128, 64, 32, 16, 8, 4};
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

 	// Map RTC interrupts to IDT
   idt[RTC_VECTOR_NUM].present = 1;
   SET_IDT_ENTRY(idt[RTC_VECTOR_NUM], rtc_handler_wrapper);
	outb((DISABLE_NMI | REG_B), SELECT_REG); 	// select B and disable NMI
	prev_data = inb(CMOS_RTC_PORT);				// get current values of B
	outb((DISABLE_NMI | REG_B), SELECT_REG);	// set index again (a read resets the index to register D)
	outb((prev_data | PERIODIC), CMOS_RTC_PORT);	// turn on bit 6 of reg B
	set_frequency(DEFAULT_FREQ); // default should be 2Hz

	// initialize local variables
	interrupt_flag = 1; // initialize to 1
	// rtc_fd = -1; // initialize to no file descriptor in use
	enable_irq(RTC_IRQ);	// enable PIC to accept interrupts
	restore_flags(flags);
}

/* JC
 * rtc_handler
 * 	DESCRIPTION:
 *			This function is called when an RTC interrupt occurs.
 *			Add functionality to make the handler to do additional tasks
 *				after each itnerrupt.
 * 	INPUT: none
 *		OUTPUT: none
 *		RETURN VALUE: none
 *		SIDE EFFECTS: none
 *
 */
void rtc_handler(void)
{
	send_eoi(RTC_IRQ);	// tell PIC to continue with it's work
	/* Don't touch anything above */

	// INSERT HERE FOR THE HANDLER TO DO SOMETHING OR UNCOMMENT
	// print_time();	// this one looks cooler
	// test_interrupts();	// this one looks like a rave
	// call terminal's write

	/* Don't touch anything below */
	// alternate between then numbers so that rtc_read doesn't need to assign
	interrupt_flag = 1;

	// Register C needs to be read after an IRQ 8 otherwise IRQ won't happen again
	outb(REG_C, SELECT_REG);
	inb(CMOS_RTC_PORT);	// throw away data
}

/* JC
 * set_frequency
 *		DESCRIPTION:
 *			Given a rate, the function will change how frequent interrupts
 *			will occur. The frequency is in powers of 2, with maximum value of 32768, but
 *			RTC device itself can only interrupt up to 8192Hz, which is rate 3. The higher the
 *			rate the less frequent the interrupts.
 *			This function wil limit the frequency up to 1024 interrupts per second.
 *			This is a rate of 6. So anything less than 6 will be forced to 6.
 *
 *			The way this function is set up, if the input frequency isn't a power of two
 *			or doesn't exist in the table, the frequency will default to 2Hz.
 *		INPUT: rate - passes in a value from 6 to 15, to determine
 *			how frequent the interrupts should occur.
 *		OUTPUT: none
 *		RETURN VALUE: none
 *		SIDE EFFECTS: changes interrupt frequency based on the calculation
 *			frequency = 32768 >> (rate-1)
 *
 */
void set_frequency(uint32_t frequency)
{
	for(curr_rate = 0; curr_rate < NUM_FREQ; curr_rate++)
		if(frequency == frequencies[curr_rate]) // find the frequency
			break;
	curr_rate++; // need to increment for calculations
	// check if it's within range
	if(curr_rate > MIN_RATE)
		curr_rate = MIN_RATE;	// can't be over 15

	if(curr_rate < MAX_RATE) // can't be less than 6
		curr_rate = MAX_RATE; // forced to 1024 Hz

	curr_frequency = frequencies[curr_rate]; // save the frequency value

	uint32_t flags;
	int8_t prev_data;
	// lock it
	cli_and_save(flags);
	// setting frequency
	outb((DISABLE_NMI | REG_A), SELECT_REG);	// select register A
	prev_data = inb(CMOS_RTC_PORT);	// get current data of A
	outb((DISABLE_NMI | REG_A), SELECT_REG);	// get A again
	outb(((prev_data & 0xF0) | curr_rate), CMOS_RTC_PORT);	// write new rate
	// unlock it
	restore_flags(flags);
}
/*********************************************************/

/* JC
 * rtc_open
 * 	DESCRIPTION:
 *			Allocates a file descriptor for the RTC.
 *		INPUT: none
 *		RETURN VALUE:
 *			-1 - couldn't open, no fd available
 *			fd - successfully created a file descriptor, return fd index
 *		SIDE_EFFECTS: opens an fd
 *
 *		NOTE TO SELF:
 *			Figure out what it's suppose to return, should it always be 0?
 *			If so then fd should be contained somewhere
 *			unless it doesn't use an fd
 */
int32_t rtc_open(const uint8_t* blank1)
{
	// Upon open it should be frequency 2Hz
	set_frequency(2);

	int32_t fd_index = get_fd_index(); // get an available index
	if(fd_index == -1)
	{
		printf("No Available FD, rtc_open\n");
		return -1;
	}

	// fill in the descriptor
	fd_t rtc_fd_info;
	rtc_fd_info.fd_jump = &rtc_ops_table; // pointer to distinct ops
	rtc_fd_info.inode_ptr = -1; // not a normal file
	rtc_fd_info.file_position = 0;
	rtc_fd_info.flags = 1;	// in use
	set_fd_info(fd_index, rtc_fd_info);

	return fd_index;
}

/* JC
 * rtc_read
 * 	DESCRIPTION:
 *			Returns once the next RTC interrupt occurs.
 *		INPUT: none
 *		OUTPUT: none
 *		RETURN VALUE: 0 - always
 *		SIDE_EFFECTS: none
 */
int32_t rtc_read(int32_t fd, uint8_t* blank1, int32_t blank2)
{
	interrupt_flag = 0;
	// wait for interrupt to happen
	while(!interrupt_flag);
	return 0;
}

/* JC
 * rtc_write
 * 	DESCRIPTION:
 *			Given a pointer to a frequency, change the frequency of the RTC driver.
 *		INPUT:
 *			fd - the file descriptor that contains rtc
 *			buf - a pointer to a 4 byte frequency
 *		OUTPUT: none
 *		RETURN VALUE:
 *			 0 - successful change
 *		SIDE_EFFECTS: modifies RTC frequency
 */
int32_t rtc_write(int32_t fd, const void* buf, int32_t blank1)
{
	uint32_t* speed = (uint32_t*)buf; // change into meaningful data
	set_frequency(*speed);
	return 0;
}

/* JC
 * rtc_close
 * 	DESCRIPTION:
 *			Closes the specified fd if it's valid. Simply turns flag into not in use.
 *		INPUT:
 *			fd - the file descriptor to close.
 *		OUTPUT: none
 *		RETURN VALUE:
 *			-1 - invalid fd
 *			 0 - successful close
 *		SIDE_EFFECTS: closes an fd
 */
int32_t rtc_close(int32_t fd)
{
	if(fd < FIRST_VALID_INDEX || fd > MAX_OPEN_FILES)
	{
		printf("Invalid FD, rtc_close\n");
		return -1; // can't close index 0 and index 1
	}

	close_fd(fd);
	return 0; // should only return 0 for checkpoint 2
}

/* IGNORE STUFF BELOW */
/* JC
 * get_update_flag - helper
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
 * update_time - helper
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
 *				binary to real current time.
 * 	INPUT: none
 *		OUTPUT: none
 *		RETURN VALUE: none
 *		SIDE EFFECTS: none
 *
 */
void binary_to_real_time(void)
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
	binary_to_real_time();
	printf("second: %d ", second);
	printf("minute: %d ", minute);
	printf("hour: %d ", hour);
	printf("day: %d ", day);
	printf("month: %d ", month);
	printf("year: %d\n", year);
}
