/* JC
 * testcases3_2.c
 *
 */

#include "testcases3_2.h"

/* JC
 * test_file_data
 *	DESCRIPTION: prints out the given index's file data
 *	Called by keyboard.c - naturally wraps around the number of dentries.
 *
 *
 */
void test_file_data(int index)
{
	// Testing read from functionality.
	uint32_t buffer_size = 500;		// modify if necessary
	uint32_t bytes_to_read = 500;		// modify if necessary
	int8_t buffer[buffer_size];
	int8_t* fname;

	fname = get_entry_name(index);
	print_file_text(fname, buffer, bytes_to_read);
}

/* JC
 *	print_file_text
 *		DESCRIPTION:
 *			Given a name, buffer, and number of bytes to read. The function
 *			will find the name, if it exists, in the filesystem and prints the text
 *			onto the screen then print the file name.
 *		INPUT:
 *			name - the file's name
 *			buffer - buffer to put info into
 *			nbytes - the max number of bytes to read at a time
 *		RETURN VALUE: none
 *
 */
void print_file_text(int8_t* name, int8_t* buffer, int32_t nbytes)
{
	// open file driver
	int32_t myfd = file_open((uint8_t*)name); // open the file

	if(myfd != -1) // file opened
	{
		int32_t retval; // don't take out

		// the while loop keeps reading until there's nothing to read or there's an error
		while((retval = file_read(myfd, (uint8_t*)buffer, nbytes)) > 0) // read stuff
			terminal_write(1, (void*)buffer, retval);

		// close current file
		file_close(myfd); // close it

		printf("\nfile name: "); // this is causing problems
		print_name(name, MAX_NAME_CHARACTERS); // wrote our own function to print name
		// different from terminal write
	}
	else
		printf("file doesn't exist.\n");
}

/* JC
 * print_freq
 *		DESCRIPTION:
 *			Uses the rtc driver to modify the frequency.
 *			requires an uncommenting in the rtc interupt handler to
 *			print out the 1's and visually see the frequency
 *		INPUT: none
 *		RETURN VALUE: none
 *
 */
void print_freq(uint32_t rate)
{
	uint32_t freq = HIGHEST_FREQ;
	uint32_t curr_freq;
	int32_t filed = rtc_open((uint8_t*)"rtc"); // open it
	if(filed != -1)
	{
		/* Test Writing Frequency - to test frequency visually uncomment print time in interrupt handler */
		clear();
		curr_freq = freq >> rate;
		printf("Frequency at: %d\n", curr_freq);
		rtc_write(filed, (void*)(&curr_freq), 1); // change the frequency

		rtc_close(filed); // always close
	}
	else
		printf("couldn't open\n");
}
