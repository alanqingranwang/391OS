/* JC
 * testcases3_2.c
 *
 */

#include "testcases3_2.h"


void collective_test32()
{
	clear();

	int8_t buffer[5000];
	print_file_text("verylargetextwithverylongname.txt", buffer, 5000);


	// print_file_info(); // this prints the file info

}


/*
 * print_file_text
 * Prints all the text from a file
 *
 */
void print_file_text(int8_t* name, int8_t* buffer, int32_t nbytes)
{
	op_data_t file_pack;
	file_pack.filename = name;
	file_pack.buf = (void*)buffer;
	file_pack.nbytes = nbytes;
	int32_t myfd = file_driver(OPEN, file_pack); // open the file
	if(myfd != -1) // file opened
	{
		file_pack.fd = myfd;
		int32_t retval;
		int32_t char_count;
		while((retval = file_driver(READ, file_pack)) > 0) // read stuff
		{
			for(char_count = 0; char_count < retval; char_count++)
			{
				if((char_count%SCREEN_CHAR) == 0)
					printf("\n");
				printf("%c", buffer[char_count]);
			}
		}
		file_driver(CLOSE, file_pack); // close it

		printf("\n\nfile name: %s", file_pack.filename);
	}
}

// trying to find rtc
void find_something()
{
	/* Testing Something */
	// print_file_info();
	// print_file_text("verylargetextwithverylongname.txt");


	// printf("%s", (entries[0]).file_name);
	// printf("\nWe have %d directory entries.\n", boot_block->num_dir_entries);
	// printf("We have %d inodes.\n", boot_block->N);
	// printf("We have %d data blocks.\n", boot_block->D);
	/* Ending Test Something */
}



	/* Testing the RTC driver */
	// op_data_t package; // create the data package, check out filesystem for struct info
	// int32_t filed = rtc_driver(OPEN, package); // open it
	// if(filed != -1)
	// {
	// 	package.fd = filed;

	// 	/* Test Writing Frequency - to test frequency visually uncomment print time in interrupt handler */
	// 	// uint32_t frequency = 1024;
	// 	// package.buf = (void*)(&frequency);
	// 	// rtc_driver(WRITE, package);

	// 	/* Test Reading */
	// 	// uint32_t count; // if it prints at the same frequency as interrupt then it works
	// 	// for(count = 0; count < 10; count ++)
	// 	// {
	// 	// 	rtc_driver(READ, package);
	// 	// 	printf("Finished reading\n");
	// 	// }

	// 	rtc_driver(CLOSE, package);
	// }
	// else
	// 	printf("couldn't open\n");

