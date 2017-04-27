/* JC
 * fd_table.c - File descriptor information
 *
 */

#include "fd_table.h"
#include "syscall.h"
#include "keyboard.h" // need driver, stdin
#include "terminal.h" // need driver, stdout
#include "rtc.h"
#include "filesystem.h"

/* JC
 * fops_table_init
 *		DESCRIPTION:
 *			Initialize each distinct fops structures
 *			Check code to see which ones are available.
 *		INPUT: none
 *		RETURN VALUE: none
 */
void fops_table_init()
{
	// rtc jump table
	rtc_ops_table.open = rtc_open;
	rtc_ops_table.read = rtc_read;	
	rtc_ops_table.write = rtc_write;
	rtc_ops_table.close = rtc_close;
	// file jump table
	filesys_ops_table.open = file_open;
	filesys_ops_table.read = file_read;
	filesys_ops_table.write = file_write;
	filesys_ops_table.close = file_close;
	// directory jump table
	dir_ops_table.open = dir_open;
	dir_ops_table.read = dir_read;
	dir_ops_table.write = dir_write;
	dir_ops_table.close = dir_close;
	// terminal jump table
	term_ops_table.open = terminal_open;
	term_ops_table.read = terminal_read;
	term_ops_table.write = terminal_write;
	term_ops_table.close = terminal_close;
	// keyboard jump table
	kybd_ops_table.open = keyboard_open;
	kybd_ops_table.read = keyboard_read;
	kybd_ops_table.write = keyboard_write;
	kybd_ops_table.close = keyboard_close;
}

/* JC
 * fd_table_init
 *		DESCRIPTION:
 *			Given the fd_table of a specific process. Initialize it to all off. Then
 *			initialize the fd index 0 and 1 to be used by terminal_driver and keyboard_driver
 *		INPUT:
 *			new_table - a pointer to the table we are trying to initialize
 *		Return value: none
 *
 */
void fd_table_init(fd_t* new_table)
{
	uint32_t table_loop;
	for(table_loop = 0; table_loop < MAX_OPEN_FILES; table_loop++)
	{
		// initialize to empty fd structure
		(new_table[table_loop]).fd_jump = NULL;
		(new_table[table_loop]).inode_ptr = -1;
		(new_table[table_loop]).file_position = 0;
		(new_table[table_loop]).flags = FD_OFF; // initialize all not in use.
	}

	// open stdin
	(new_table[STDOUT_]).flags = FD_ON;
	(new_table[STDOUT_]).fd_jump = &term_ops_table;
	// open stdout
	(new_table[STDIN_]).flags = FD_ON;
	(new_table[STDIN_]).fd_jump = &kybd_ops_table;
}

/* JC
 * close_all_fd
 *		DESCRIPTION: Go through all the fds and turn them off
 *		INPUT: none
 *		RETURN VALUE: none
 */
void close_all_fd()
{
	uint32_t halt_cnt;
	// close all the fd
	for(halt_cnt = 0; halt_cnt < MAX_OPEN_FILES; halt_cnt++)
	{
		(((process_array[current_process[curr_terminal]])->fd_table)[halt_cnt]).fd_jump = NULL;
		(((process_array[current_process[curr_terminal]])->fd_table)[halt_cnt]).inode_ptr = -1;
		(((process_array[current_process[curr_terminal]])->fd_table)[halt_cnt]).file_position = 0;
		(((process_array[current_process[curr_terminal]])->fd_table)[halt_cnt]).flags = FD_OFF;
	}
}

/* JC
 * get_fd_index
 * 	DESCRIPTION:
 *			Finds an available file descriptor in the table to use.
 *			Preventing interrupts should be the job of the user.
 *		INPUT: none
 *		RETURN VALUE:
 *			index of available descriptor
 *			-1 - no available descriptor
 *
 */
int32_t get_fd_index()
{
	uint32_t table_loop;
	// should not consider index 0 and 1
	for(table_loop = FIRST_VALID_INDEX; table_loop < MAX_OPEN_FILES; table_loop++)
	{
		if((((process_array[current_process[curr_terminal]])->fd_table)[table_loop]).flags == FD_OFF) // available index
			return table_loop;
	}

	return -1; // none available
}

/* JC
 * set_fd_info
 *		DESCRIPTION:
 *			A pair function that goes with get_fd_index.
 *			When the user gets an index, they pass in a fd_t struct that contains
 *			the information they want to fill at the index given.
 *		INPUT:
 *			index - the fd table index that needs to contain the file descriptor info
 *			file_info - the file that needs to be added to the index
 *		RETURN VALUE: none
 *
 */
int32_t set_fd_info(int32_t index, fd_t file_info)
{
	if(index < 0 || index >= MAX_OPEN_FILES)
		return -1;

	// fill the table index with the given info
	(((process_array[current_process[curr_terminal]])->fd_table)[index]).fd_jump = file_info.fd_jump;
	(((process_array[current_process[curr_terminal]])->fd_table)[index]).inode_ptr = file_info.inode_ptr;
	(((process_array[current_process[curr_terminal]])->fd_table)[index]).file_position = file_info.file_position;
	(((process_array[current_process[curr_terminal]])->fd_table)[index]).flags = file_info.flags;

	return 0;
}

/* JC
 * get_inode_ptr
 *		DESCRIPTION:
 *			Retrieves the inode_ptr for a given index
 *		INPUT:
 *			index - the fd table index that contains the inode_ptr
 *		RETURN VALUE: inode_ptr that is asked
 *
 */
int32_t get_inode_ptr(int32_t index)
{
	if(index < 0 || index >= MAX_OPEN_FILES)
		return -1;

	// get the ptr to the inode
	return (((process_array[current_process[curr_terminal]])->fd_table)[index]).inode_ptr;
}

/* JC
 * close_fd
 *		DESCRIPTION:
 *			closes the specified file descriptor
 *		INPUT:
 *			index - the index we want to close
 *		RETURN VALUE: none
 *
 */
void close_fd(int32_t index)
{
	if(index < 0 || index >= MAX_OPEN_FILES)
		return;
	
	// close everything
	(((process_array[current_process[curr_terminal]])->fd_table)[index]).fd_jump = NULL;
	(((process_array[current_process[curr_terminal]])->fd_table)[index]).inode_ptr = -1;
	(((process_array[current_process[curr_terminal]])->fd_table)[index]).file_position = 0;
	(((process_array[current_process[curr_terminal]])->fd_table)[index]).flags = FD_OFF;
}

/*	JC
 *	get_file_position
 *		DESCRIPTION:
 *			returns the file position of the given index
 *		INPUT:
 *			index - the index we want to find the file position of.
 *		RETURN VALUE: none
 *
 */
uint32_t get_file_position(int32_t index)
{
	return (((process_array[current_process[curr_terminal]])->fd_table)[index]).file_position;
}

/* JC
 * add_offset
 *		DESCRIPTION:
 *			updates the file's offset after a read
 *		INPUT:
 *			index - the file we want to update
 *			amt - the amount it should be incremented by
 *		RETURN VALUE: none
 *
 */
void add_offset(int32_t index, uint32_t amt)
{
	if(index < 0 || index >= MAX_OPEN_FILES)
		return;
	
	(((process_array[current_process[curr_terminal]])->fd_table)[index]).file_position += amt;
}

/* JC
 * check_valid_fd
 *		DESCRIPTION:
 *			Check if the index is on.
 *		INPUT:
 *			index - the file descriptor we want to check
 *		RETURN VALUE: 0 for invalid, 1 for valid
 */
int32_t check_valid_fd(int32_t index)
{
	// fd is in use
	if(index < 0 || index >= MAX_OPEN_FILES)
		return 0;

	return (((process_array[current_process[curr_terminal]])->fd_table)[index]).flags;
}

