/* JC
 * terminal.c - Contains the terminal driver.
 */

#include "terminal.h"

/* JC
 * terminal_driver
 *		DESCRIPTION:
 *			The driver for the terminal to execute the proper operation
 *		INPUT:
 *			cmd - the operation we should be executing
 *		RETURN VALUE:
 *			-1 - incorrect cmd or failure from operations
 *			returns are dependent on operation, check interfaces
 */
int32_t terminal_driver(int cmd, op_data_t input){
	switch(cmd){
		case OPEN:
			return terminal_open();
		case CLOSE:
			return terminal_close(STDOUT_FD);
		case READ:
			return terminal_read(STDOUT_FD, (int8_t*)(input.buf), input.nbytes);
		case WRITE:
			return terminal_write(STDOUT_FD, (int8_t*)(input.buf), input.nbytes);
		default:
			return -1;
	}
}

/* JC
 * terminal_open
 *		DESCRIPTION:
 *			Opens the terminal for use, a driver operation. Allocats the fd 1 (stdout)
 *			Should always be open till end of kernel.
 *		INPUT:
 *			none
 *		RETURN VALUE:
 *			0 - success
 */
int32_t terminal_open(){
	return 0;
}

/* JC
 * terminal_close
 *		DESCRIPTION:
 *			Closes the terminal from use, a driver operation
 *		INPUT:
 *			fd - 1, stdout should always be open until end of kernel.
 *		RETURN VALUE:
 *			0 - success
 */
int32_t terminal_close(int32_t fd){
	return 0;
}

/* JC
 * terminal_read
 *		DESCRIPTION:
 *			Reads the buffer passed in for nbytes. Then interprets the information
 *			if meaningful. For example, keyboard types ls, presses enter and calls this.
 *			Terminal read interprets it to print out file info.
 *		INPUT:
 *			fd - should be 1 (stdout)
 *			buf - the buffer we are trying to interpret
 *			nbytes - number of bytes we are interpreting
 *		RETURN VALUE:
 *			-1 - failure
 *			 0 - sucess
 */
int32_t terminal_read(int32_t fd, int8_t* buf, int32_t nbytes){
	return -1;
}

/* JC
 * terminal_write
 *		DESCRIPTION:
 *			Writes nbytes of char from the buffer to the screen through putc.
 *		INPUT:
 *			fd - 1 (stdout)
 *			buf - the buffer we are trying to write to screen
 *			nbytes - the number of bytes we are trying to write to screen
 *		RETURN VALUE:
 *			-1 failure
 *			otherwise number of bytes written
 */
int32_t terminal_write(int32_t fd, int8_t* buf, int32_t nbytes){
	int32_t success = 0;

	int32_t i;
	for(i = 0; i < nbytes; i++){
		putc(buf[i]);
		success++;
	}
	return success;

	return -1;
}
