/* JC
 * fd_table.c - File descriptor information
 *
 */

#include "fd_table.h"

/* File Descriptor Table */
static fd_t fd_table[MAX_OPEN_FILES];

/* JC
 * Initializes the file descriptor table, will migrate to the execute syscall
 *		When we start creating multiple processes.
 */
void fd_table_init()
{
	uint32_t flags;
	cli_and_save(flags);

	uint32_t table_loop;
	for(table_loop = 0; table_loop < MAX_OPEN_FILES; table_loop++)
	{
		(fd_table[table_loop]).flags = FD_OFF; // initialize all not in use.
	}

	// open stdin
	// open stdout
	restore_flags(flags);
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
		if((fd_table[table_loop]).flags == FD_OFF) // available index
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
	(fd_table[index]).file_op_table_ptr = file_info.file_op_table_ptr;
	(fd_table[index]).inode_ptr = file_info.inode_ptr;
	(fd_table[index]).file_position = file_info.file_position;
	(fd_table[index]).flags = file_info.flags;
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
	return fd_table[index].inode_ptr;
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
	fd_table[index].flags = FD_OFF;
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
	return fd_table[index].file_position;
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
	fd_table[index].file_position += amt;
}


