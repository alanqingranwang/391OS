/* JC
 * testcases3_2.c
 *
 */

#include "testcases3_2.h"

// remove this after checkpoint 2, and all it's uses
static int print_one = 0;
// retrieves veriable to check to print one
int get_print_one()
{
	return print_one;
}

void set_print_one(int change)
{
	print_one = change;
}

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
 *	print_file_text - NOT FULLY FUNCTIONAL I NEED TERMINAL CODE
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
	op_data_t file_pack;
	file_pack.filename = name;
	file_pack.buf = (void*)buffer;
	file_pack.nbytes = nbytes;	// how many bytes to read at a time
	int32_t myfd = file_driver(OPEN, file_pack); // open the file
	// open terminal driver
	op_data_t term_pack;
	terminal_driver(OPEN, term_pack);

	if(myfd != -1) // file opened
	{
		file_pack.fd = myfd;
		int32_t retval; // don't take out

		// the while loop keeps reading until there's nothing to read or there's an error
		while((retval = file_driver(READ, file_pack)) > 0) // read stuff
		{
			// following for loop pritns out the buffer, but should be replaced
			// with openning terminal driver and calling write
			// terminal should be doing the writing or reading or what not
			term_pack.buf = file_pack.buf;
			term_pack.nbytes = (uint32_t)retval;
			terminal_driver(WRITE, term_pack);
		}

		// close current file
		file_driver(CLOSE, file_pack); // close it

		printf("\nfile name: "); // this is causing problems
		print_name(file_pack.filename); // wrote our own function to print name
		// different from terminal write
	}
	else
	{
		printf("file doesn't exist.\n");
	}
	// close terminal driver
	terminal_driver(CLOSE, term_pack);
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
	op_data_t rtc_pack; // create the data package, check out filesystem for struct info
	int32_t filed = rtc_driver(OPEN, rtc_pack); // open it
	if(filed != -1)
	{
		/* Test Writing Frequency - to test frequency visually uncomment print time in interrupt handler */
		clear();
		curr_freq = freq >> rate;
		printf("Frequency at: %d\n", curr_freq);
		rtc_pack.buf = (void*)(&curr_freq);
		rtc_driver(WRITE, rtc_pack); // change the frequency

		/* Test Reading */ // recommend testing at 2Hz - or check out my code
		// uint32_t count; // if it prints at the same frequency as interrupt then it works
		// for(count = 0; count < 10; count ++)
		// {
		// 	rtc_driver(READ, package);
		// 	printf("Finished reading\n");
		// }

		rtc_driver(CLOSE, rtc_pack); // always close
	}
	else
		printf("couldn't open\n");
}


