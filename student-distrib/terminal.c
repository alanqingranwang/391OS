/* NM
 * terminal.c - Contains the terminal driver.
 */

#include "terminal.h"
static int8_t save_buff[TERM_BUFF_SIZE];

/* NM
 * terminal_open
 *		DESCRIPTION:
 *			Opens the terminal for use, a driver operation. Allocats the fd 1 (stdout)
 *			Should always be open till end of kernel.
 *		INPUT:
 *			none
 *		RETURN VALUE:
 *			0 - success
 */
int32_t terminal_open(const uint8_t* blank){
	return -1;
}

/* NM
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

int32_t terminal_read(int32_t fd, uint8_t* buf, int32_t nbytes){
	int32_t i;
	for(i = 0; i<TERM_BUFF_SIZE; i++)
		save_buff[i] = ' ';
	int32_t success = 0;
	for(i = 0; (i < TERM_BUFF_SIZE) && (i < nbytes); i++){
		save_buff[i] = buf[i];
		success++;
	}
	return -1; // not suppose to be able to read
	// return success;
}

/* NM
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
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes){
	uint8_t* buffer = (uint8_t*)buf;
	int32_t i=0;
	for(i = 0; i < nbytes && i < TERM_BUFF_SIZE; i++){
		putc(buffer[i]);
	}
	return i;
}

/* NM
 * terminal_close
 *		DESCRIPTION:
 *			Closes the terminal from use, a driver operation
 *		INPUT:
 *			fd - 1, stdout should always be open until end of kernel.
 *		RETURN VALUE:
 *			0 - success
 */
int32_t terminal_close(int32_t fd){
	return -1;
}

/* NM
 * terminal_retrieve
 *	DESCRIPTION:
 *		Puts the saved buffer from the previous output into the buf.
 *		Used by the keyboard.
 */
int32_t terminal_retrieve(uint8_t* buf, int32_t nbytes){
	int32_t i=0;
	for(i = 0; i < nbytes && i < TERM_BUFF_SIZE; i++){
		buf[i] = save_buff[i];
		if (save_buff[i] == ' ') break;
	}
	return i;
}
