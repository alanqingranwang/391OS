/* JC
 * terminal.c - Contains the terminal driver.
 */
#include "terminal.h"
static int8_t save_buff[TERM_BUFF_SIZE];
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
int32_t terminal_driver(uint32_t cmd, op_data_t input){
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
 *			Saves the buffer locally.
 *		INPUT:
 *			fd - should be 1 (stdout)
 *			buf - the buffer we are trying to interpret
 *			nbytes - number of bytes we are interpreting
 *		RETURN VALUE:
 *			 0 - sucess
 */
int32_t terminal_read(int32_t fd, int8_t* buf, int32_t nbytes){
	int32_t i;
	for(i = 0; i<TERM_BUFF_SIZE; i++)
		save_buff[i] = ' ';
	int32_t success = 0;
	for(i = 0; (i < TERM_BUFF_SIZE) && (i < nbytes); i++){
		save_buff[i] = buf[i];
		success++;
	}
	return success;
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
 *			otherwise number of bytes written
 */
int32_t terminal_write(int32_t fd, int8_t* buf, int32_t nbytes){
	int32_t i=0;
	for(i = 0; i < nbytes && i < TERM_BUFF_SIZE; i++){
		putc(buf[i]);
	}
	return i;
}

int32_t terminal_retrieve(int32_t fd, int8_t* buf, int32_t nbytes){
	int32_t i=0;
	for(i = 0; i < nbytes && i < TERM_BUFF_SIZE; i++){
		buf[i] = save_buff[i];
		if (save_buff[i] == ' ') break;
	}
	return i;
}
