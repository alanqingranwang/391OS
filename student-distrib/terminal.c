/* NM
 * terminal.c - Contains the terminal driver.
 */

#include "terminal.h"
#include "syscall.h"

static int8_t save_buff[MAX_TERMINAL][TERM_BUFF_SIZE];

/*
 * terminal_init
 *		DESCRIPTION:
 *		INPUT:
 *		RETURN VALUE:
 *
 */
int32_t terminal_init()
{
	in_use[0] = 0;
	in_use[1] = 1;
	in_use[2] = 1;

	return 0;
}

int32_t terminal_switch(uint32_t new_terminal){
	// sanity check
	if(new_terminal == curr_terminal)
		return 0;

	if(new_terminal > 2 || new_terminal < 0)
		return -1;

	curr_terminal = new_terminal;
	clear(); // clear after assigning new terminal
	int curr_process = current_process[new_terminal];

	if(curr_process == -1)
	{	// if the base shell isn't on 
		in_use[new_terminal] = 0;
		execute((uint8_t*)"shell");
	}
	else
	{
		in_use[curr_process] = 2;
		execute(process_array[curr_process]->comm);
	}

	return 0;
}


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
	if(buf == NULL)
		return -1;

	int32_t i;
	for(i = 0; i<TERM_BUFF_SIZE; i++)
		save_buff[curr_terminal][i] = '\0'; // clean the buffer
	int32_t success = 0;
	for(i = 0; (i < TERM_BUFF_SIZE) && (i < nbytes); i++){
		save_buff[curr_terminal][i] = buf[i]; // fill it
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
	if(buf == NULL)
		return -1;

	uint8_t* buffer = (uint8_t*)buf;
	int32_t i=0;
	for(i = 0; i < nbytes; i++){
		putc(buffer[i]); // output all the charactrs in the given buffer
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

/* NM, JC
 * terminal_retrieve
 *	DESCRIPTION:
 *		Puts the saved buffer from the previous output into the buf.
 *		Used by the keyboard.
 */
int32_t terminal_retrieve(uint8_t* buf, int32_t nbytes){
	int32_t i = 0; // goes through the whole save buffer
	int32_t cmd_cnt = 0; // starts filling buf from the beginning

	while(save_buff[curr_terminal][i] == ' ' && i < nbytes && i < TERM_BUFF_SIZE)
		i++; // get to the real content, strip the beginning spaces

	for(; cmd_cnt < nbytes && i < TERM_BUFF_SIZE; i++){
		buf[cmd_cnt] = save_buff[curr_terminal][i];
		if (save_buff[curr_terminal][i] == '\0') break; // I need this to not break the shell
		cmd_cnt++; // off by one, should count when it's not a space
	}

	return cmd_cnt; // how many bytes are in the buf
}
