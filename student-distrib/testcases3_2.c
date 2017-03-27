/* JC
 * testcases3_2.c
 *
 */

#include "testcases3_2.h"

/* JC
 * collective_tests32
 *		DESCRITPTION:
 *			Contains a test to print out all the file info in directory
 *						a test to print out all the files data one at a time
 *						a test to test all the frequencies.
 *		INPUT:
 *			none
 *		OUTPUT: dependent on test
 *		RETURN VALUE: none
 *
 */
void collective_test32()
{
	clear();
	uint32_t entry_amt = get_num_entries(); // used to print all the files
	uint32_t Hz = 2; // default is 2
	uint32_t wait_time = 5; // how many seconds are you willing to wait
	uint32_t file_loop;
	/* Change variables here to test various things. */

	/* Reading all the file info */
	CTR4 = 0;
	print_file_info(); // this prints the file info

	while(CTR4 == 0) {} // wait to proceed Press CTRL+4

	// Testing read from functionality. 
	uint32_t buffer_size = 500;		// modify if necessary
	uint32_t bytes_to_read = 500;		// modify if necessary
	int8_t buffer[buffer_size];
	int8_t* fname;
	
	// // read all the files in order, wait wait_time seconds between
	// for(file_loop = 0; file_loop < entry_amt; file_loop++)
	// {
	// 	clear();
	// 	fname = get_entry_name(file_loop);
	// 	print_file_text(fname, buffer, bytes_to_read);
	// 	WAIT_X_SECONDS(wait_time, Hz);		
	// }

	CTR4 = 0;
	uint32_t curr_count = 0;
	while(curr_count < entry_amt)
	{
		clear();
		fname = get_entry_name(curr_count);
		print_file_text(fname, buffer, bytes_to_read);
		curr_count++;

		while(CTR4 < curr_count) {} // wait to proceed
	}
	clear();

	// uncomment the following to test individual files
	// print_file_text("verylargetextwithverylongname.txt", buffer, bytes_to_read);


	/* Testing the Frequency */
	// print_freq();
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
		print_name(file_pack.filename, MAX_CHAR_LENGTH); // wrote our own function to print name
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
void print_freq()
{
	uint32_t freq = 2;
	uint32_t curr_freq;
	op_data_t rtc_pack; // create the data package, check out filesystem for struct info
	int32_t filed = rtc_driver(OPEN, rtc_pack); // open it
	if(filed != -1)
	{
		uint32_t freq_loop;
		/* Test Writing Frequency - to test frequency visually uncomment print time in interrupt handler */
		for(freq_loop = 0; freq_loop < HIGHEST_RATES; freq_loop++)
		{
			clear();
			curr_freq = freq << freq_loop;
			printf("Frequency at: %d\n", curr_freq);
			rtc_pack.buf = (void*)(&curr_freq);
			rtc_driver(WRITE, rtc_pack); // change the frequency
			WAIT_X_SECONDS(3, curr_freq);
		}

		clear();
		printf("Freq = 2\n");
		// change it back to 2
		rtc_pack.buf = (void*)&freq;
		rtc_driver(WRITE, rtc_pack);

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


