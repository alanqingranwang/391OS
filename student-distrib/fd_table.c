/* JC
 * fd_table.c - File descriptor information
 *
 */

#include "fd_table.h"
#include "syscall.h"
#include "keyboard.h" // need driver, stdin
#include "terminal.h" // need driver, stdout


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
		((new_table[table_loop]).fd_jump).open = NULL;
		((new_table[table_loop]).fd_jump).read = NULL;
		((new_table[table_loop]).fd_jump).write = NULL;
		((new_table[table_loop]).fd_jump).close = NULL;
		(new_table[table_loop]).inode_ptr = -1;
		(new_table[table_loop]).file_position = 0;
		(new_table[table_loop]).flags = FD_OFF; // initialize all not in use.
	}

	// open stdin
	(new_table[STDOUT_]).flags = FD_ON;
	((new_table[STDOUT_]).fd_jump).open = terminal_open;
	((new_table[STDOUT_]).fd_jump).read = terminal_read;
	((new_table[STDOUT_]).fd_jump).write = terminal_write;
	((new_table[STDOUT_]).fd_jump).close = terminal_close;
	// open stdout
	(new_table[STDIN_]).flags = FD_ON;
	((new_table[STDIN_]).fd_jump).open = keyboard_open;
	((new_table[STDIN_]).fd_jump).read = keyboard_read;
	((new_table[STDIN_]).fd_jump).write = keyboard_write;
	((new_table[STDIN_]).fd_jump).close = keyboard_close;
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
		if(((((p_c.process_array)[p_c.current_process])->fd_table)[table_loop]).flags == FD_OFF) // available index
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
void set_fd_info(int32_t index, fd_t file_info)
{
	if(index < 0 || index >= MAX_OPEN_FILES)
		return;

	(((((p_c.process_array)[p_c.current_process])->fd_table)[index]).fd_jump).open = (file_info.fd_jump).open;
	(((((p_c.process_array)[p_c.current_process])->fd_table)[index]).fd_jump).read = (file_info.fd_jump).read;
	(((((p_c.process_array)[p_c.current_process])->fd_table)[index]).fd_jump).write = (file_info.fd_jump).write;
	(((((p_c.process_array)[p_c.current_process])->fd_table)[index]).fd_jump).close = (file_info.fd_jump).close;
	((((p_c.process_array)[p_c.current_process])->fd_table)[index]).inode_ptr = file_info.inode_ptr;
	((((p_c.process_array)[p_c.current_process])->fd_table)[index]).file_position = file_info.file_position;
	((((p_c.process_array)[p_c.current_process])->fd_table)[index]).flags = file_info.flags;
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

	return ((((p_c.process_array)[p_c.current_process])->fd_table)[index]).inode_ptr;
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
	
	(((((p_c.process_array)[p_c.current_process])->fd_table)[index]).fd_jump).open = NULL;
	(((((p_c.process_array)[p_c.current_process])->fd_table)[index]).fd_jump).read = NULL;
	(((((p_c.process_array)[p_c.current_process])->fd_table)[index]).fd_jump).write = NULL;
	(((((p_c.process_array)[p_c.current_process])->fd_table)[index]).fd_jump).close = NULL;
	((((p_c.process_array)[p_c.current_process])->fd_table)[index]).inode_ptr = -1;
	((((p_c.process_array)[p_c.current_process])->fd_table)[index]).file_position = 0;
	((((p_c.process_array)[p_c.current_process])->fd_table)[index]).flags = FD_OFF;
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
	return ((((p_c.process_array)[p_c.current_process])->fd_table)[index]).file_position;
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
	
	((((p_c.process_array)[p_c.current_process])->fd_table)[index]).file_position += amt;
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

	return ((((p_c.process_array)[p_c.current_process])->fd_table)[index]).flags;
}

