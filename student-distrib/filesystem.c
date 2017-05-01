/* JC
 * filesystem.c - Contains the definitions for the files
 * tab size = 3, no space
 */
#include "filesystem.h"
#include "fd_table.h"
#include "terminal.h"

static uint32_t num_entries;
static uint32_t num_inodes;
static uint32_t num_data_blocks;

/* holds the file_name size for all the dentries, maxes at 32 chars */
static int32_t character_count[MAX_ENTRIES];

/* JC
 * filesystem_init
 * 	DESCRIPTION:
 *			Initializes the file system with the proper data to begin use.
 *			Creates an overlay of structures to make the data more useful.
 *		INPUT:
 *			boot_addr - Passed in from kernel, contains the address to the start of boot block
 *		OUTPUT: none
 *		RETURN VALUE: none
 *		SIDE EFFECTS: initializes the file system's data
 *
 */
void filesystem_init(boot_block_t* boot_addr)
{
	uint32_t flags;
	cli_and_save(flags);
	// initialize boot block pointer, and retreive all information
	boot_block = boot_addr;
	num_entries = boot_block->num_dir_entries;
	num_inodes = boot_block->N;
	num_data_blocks = boot_block->D;
	// initialize the pointer to first dentry
	entries = (dentry_t*)(&(boot_block->dir_entries));
	// initialize pointer to first inode
	inodes = (inode_t*)(boot_block+INODE_OFFSET); // should be the first block after boot
	// initialize pointer to first data block
	data_blocks = (data_block_t*)(inodes+num_inodes);
	// initiaize the file_name table
	create_char_count();

	fops_table_init();
	restore_flags(flags);
}

/************************CHECKPOINT 3.2****************************/

/* JC
 * print_file_info
 *		Prints all the file information.
 *		file name
 *		file type
 *		file size (in bytes)
 */
 void print_file_info()
 {
 	clear();
 	uint32_t d_loop = 0; // loops through all the dentry loops
 	uint32_t char_loop; // go through all the characters
 	dentry_t file_dent;

 	int32_t dir_fd = dir_open((uint8_t*)".");
 	uint8_t buffer[33];
 	int32_t nbytes = 32;

 	int32_t cnt;
 	while(0 != (cnt = dir_read(dir_fd, buffer, nbytes)))
 	{
 		read_dentry_by_index(d_loop, &file_dent); // get the dentry
 		if(cnt == -1)
 		{
 			dir_close(dir_fd);
 			printf("directory entry read failed\n");
 			return;
 		}
 		printf("file name: ");
 		terminal_write(1, buffer, cnt);
 		for(char_loop = cnt; char_loop < NUM_SPACES; char_loop++)
 			putc(' '); // add spaces to align the rest

 		printf("file type: %d ", file_dent.file_type);
 		printf("file size: %d\n", inodes[file_dent.inode_idx].file_size);
 		d_loop++;
 	}

 	dir_close(dir_fd);
 }

/* JC
 * print_name - helper
 *		DESCRIPTION:
 *			Properly prints the name of the file from dentry.
 *		INPUT:
 *			buf - the name of the file
 *		RETURN VALUE: returns the number of chars written
 *		SIDE EFFECTS: putc to the screen
 *
 */
int32_t print_name(int8_t* buf, int32_t max_char)
{
	int32_t i = 0;
	while(buf[i] != '\0' && i < max_char)
	{
		putc(buf[i]);
		i++;
	}
	return i;
}

/**********************Directory Driver****************************/

/* JC
 *	dir_open
 *	DESCRIPTION:
 *		Allocates a pointer on the file descriptor table for the given process.
 *	INPUT:
 *		filename - should be the name of the directory
 *	OUTPUT:
 *		Only possible error checking
 *	RETURN VALUE:
 *		-1 - invalid, or unable to open
 *		fd_index - the index holding the file descriptor.
 */
int32_t dir_open(const uint8_t* filename)
{
	int32_t fd_index = get_fd_index(); // get an available index
	if(fd_index == -1)
	{
		printf("No Available FD, dir_open\n");
		return -1;
	}

	// fill in the descriptor
	fd_t dir_fd_info;
	dir_fd_info.fd_jump = &dir_ops_table;
	dir_fd_info.inode_ptr = -1;
	dir_fd_info.file_position = 0; // start offset at 0
	dir_fd_info.flags = FD_ON;	// in use
	set_fd_info(fd_index, dir_fd_info);

	return fd_index;
}

/* JC
 *	dir_read
 *	DESCRIPTION:
 *		The FD offset represents which file we are at in dentry.
 *	INPUT:
 *		Read nbytes number of characters or the file name and place them into the buf.
 *	OUTPUT:
 *		none
 *	RETURN VALUE:
 *		bytes_read - 0 if there's nothing left to read, specifically when the index
 *							of dentry is greater than the number of entries we have.
 */
int32_t dir_read(int32_t fd, uint8_t* buf, int32_t nbytes)
{
	int32_t bytes_read = 0; // 0 bytes read
	int32_t dent_index = get_file_position(fd); // get the current file index

	if(dent_index >= num_entries)
		return bytes_read; // no more files

	// read the whole name, or up to nbytes
	while((entries[dent_index].file_name)[bytes_read] != '\0' && bytes_read < nbytes && bytes_read < MAX_NAME_CHARACTERS)
	{
		buf[bytes_read] = (entries[dent_index].file_name)[bytes_read];
		bytes_read++;
	}

	add_offset(fd, 1); // increment offset to next file
	return bytes_read;
}

/* JC
 *	dir_write
 *	DESCRIPTION:
 *		The directory is read only, do nothing
 *	INPUT:
 *		none
 *	OUTPUT:
 *		Error message
 *	RETURN VALUE:
 *		-1 - can't write to directory
 */
int32_t dir_write(int32_t fd, const void* blank1, int32_t blank2)
{
	printf("READ ONLY DIRECTORY\n");
	return -1;
}

/* JC
 *	dir_close
 *	DESCRIPTION:
 *		Close the given fd index
 *	INPUT:
 *		fd - the index the directory was opened in
 *	OUTPUT:
 *		error message
 *	RETURN VALUE:
 *		0 - successful close
 *		-1 - can't close the given fd, invalid
 */
int32_t dir_close(int32_t fd)
{
	close_fd(fd);	// close the portal
	return 0;
}

/***********************File Driver*******************************/

/* JC
 * file_open
 * 	DESCRIPTION:
 *			Finds the filename in the file system, then creates a file descriptor for it.
 *		INPUT:
 *			filename - the filename we are trying to open
 *		OUTPUT: none
 *		RETURN VALUE: the fd index
 *
 */
int32_t file_open(const uint8_t* filename)
{
	// check if the file name exists
	dentry_t my_dentry;
	read_dentry_by_name(filename, &my_dentry);

	// name is valid
	int32_t fd_index = get_fd_index(); // get an available index
	if(fd_index == -1)
	{
		printf("No Available FD, file_open\n");
		return -1;
	}

	// fill in the descriptor
	fd_t file_fd_info;
	file_fd_info.fd_jump = &filesys_ops_table;
	file_fd_info.inode_ptr = my_dentry.inode_idx;
	file_fd_info.file_position = 0; // start offset at 0
	file_fd_info.flags = FD_ON;	// in use
	set_fd_info(fd_index, file_fd_info);

	return fd_index;
}

/* JC
 * file_read
 *		DESCRIPTION:
 *			The file driver's read operation, given a buffer, and nbytes to read.
 *			It returns the number of bytes that have been read.
 *		INPUT:
 *			fd - the file's file descriptor
 *			buf - the buffer we want to fill
 *			nbytes - the number of bytes trying to be read
 *		RETURN VALUE:
 *			the number of bytes that were actually read
 *			 0 - nothing read, nothing bad happened
 *			-1 - something wrong happened, couldn't read
 *
 */
int32_t file_read(int32_t fd, uint8_t* buf, int32_t nbytes)
{
	uint32_t read_amt = read_data(get_inode_ptr(fd), get_file_position(fd), buf, nbytes);
	if(read_amt > 0)	// if there's an amount then increment
		add_offset(fd, read_amt);
	return read_amt;	// return the response
}

/* JC
 * file_write
 *		DESCRIPTION:
 *			The file driver's write operation, should just return because because
 *			The filesystem is read only, no writing.
 *		INPUT: none
 *		RETURN VALUE:
 *			-1 - couldn't read
 *
 */
int32_t file_write(int32_t fd, const void* blank1, int32_t blank2)
{
	printf("READ ONLY FILES\n");
	return -1;
}

/* JC
 *	file_close
 *		DESCRIPTION:
 *			The file driver's close operation, given the file's index. This close
 *			the file.
 *		INPUT:
 *			fd - the file that we are trying to close
 *		RETURN VALUE:
 *			-1 - invalid close fd
 *			 0 - success
 *
 */
int32_t file_close(int32_t fd)
{
	close_fd(fd);	// close the portal
	return 0;
}

/*************************************************************/
/* JC
 * create_char_count
 * 	DESCRIPTION:
 *			Goes through all the dentry names and stores the sizes, with
 *			the size maxing at 32 chars.
 *		if it was possible to load new files, we need a way to update the table
 *		INPUT: none
 *		OUTPUT: none
 *		RETURN VALUE: none
 *		SIDE EFFECTS: none
 *
 */
void create_char_count()
{
	// register keyword puts the variable into a register for faster access
	register uint32_t len;
	register uint32_t dentry_loop;
	// access each file name
	for(dentry_loop = 0; dentry_loop < MAX_ENTRIES; dentry_loop++)
	{
		len = 0;
		// Count how many characters are in the current dentry's name
		while(((entries[dentry_loop]).file_name)[len] != '\0'
				&& len < MAX_NAME_CHARACTERS)
			len++;
		character_count[dentry_loop] = len; // hold count for future functions
	}
}

/* JC
 * read_dentry_by_name
 * 	DESCRIPTION:
 *			Use the pointer to the start of dentry like an array to access
 *			each entry's file name. Find the matching file name if it exists.
 *			return with the status of the find. Fills in the dentry structure
 *			pointer if a match is found.
 *		INPUT:
 *			fname - the file name, string, that we need to find
 *			dentry - a pointer to the dentry structure that should be filled
 *		OUTPUT: none
 *		RETURN VALUE: -1 - Failure (non-existent file)
 *						   0 - Success (found the file)
 *		SIDE EFFECTS: none
 *
 */
int32_t read_dentry_by_name(const uint8_t *fname, dentry_t *dentry)
{
	int32_t entry_name_len; // holds how long the dentry's filename is
	int32_t dentry_loop;
	int32_t given_name_len = 0;
	// Count how many characters are in the current dentry's name
	while((fname[given_name_len] != ' ') && (fname[given_name_len] != '\0')
			&& (given_name_len < MAX_NAME_CHARACTERS))
		given_name_len++;

	if(given_name_len == 0)
		return -1; // empty string

	if(given_name_len > MAX_NAME_CHARACTERS)
		given_name_len = MAX_NAME_CHARACTERS; // pretend as if it's the same size.
	// go through all the dentries
	for(dentry_loop = 0; dentry_loop < MAX_ENTRIES; dentry_loop++)
	{
		// get the size of the entry's name from the count table
		entry_name_len = character_count[dentry_loop];
		// no point in checking if not the same length
		if(given_name_len == entry_name_len)
		{
			// if the strings are the same, copy over data
			if(strncmp((int8_t*)fname, (entries[dentry_loop]).file_name, entry_name_len) == 0)
			{
				strncpy(dentry->file_name, (entries[dentry_loop]).file_name, entry_name_len); // (dest, src)
				dentry->file_type = (entries[dentry_loop]).file_type;
				dentry->inode_idx = (entries[dentry_loop]).inode_idx;
				return 0; // found the entry
			}
		}
	}
	return -1;
}

/* JC
 * read_dentry_by_index
 * 	DESCRIPTION:
 *			Uses the entries pointer and the given index to find the correct dentry.
 *			Takes the data and fills the given pointer to another dentry.
 *		INPUT:
 *			index - the index for dentry that we want
 *			dentry - a pointer to the dentry structure that should be filled in
 *		OUTPUT: none
 *		RETURN VALUE: -1 - Failure (invalid index)
 *							0 - Success
 *		SIDE EFFECTS: none
 *
 */
int32_t read_dentry_by_index(uint32_t index, dentry_t *dentry)
{
	if(index > MAX_ENTRIES)
		return -1; // invalid index
	// copy over the data
	strncpy(dentry->file_name, (entries[index]).file_name, character_count[index]);
	dentry->file_type = (entries[index]).file_type;
	dentry->inode_idx = (entries[index]).inode_idx;
	return 0;
}

/* JC
 * Returns the number of entries in the filesystem.
 */
uint32_t get_num_entries()
{
	return num_entries;
}

/* JC
 * Returns the name of the entry in the filesystem
 */
int8_t* get_entry_name(uint32_t index)
{
	return (entries[index]).file_name;
}

/* JC
 * read_data
 * 	DESCRIPTION:
 *			Reads up to 'length' number of bytes. starting from position offset.
 *			in the file with inode number inode and returning the number of bytes
 *			read and placed in the buffer.
 *		INPUT:
 *			inode - the inode index, that we want
 *			offset - offset from the start of file
 *			buf - the buffer we should fill with read data
 *			length - How many bytes to read from offset
 *		OUTPUT:
 *		RETURN VALUE: -1 - Failure (invalid inode number)
 *									(invalid data block index in the inode)
 *							return the number of bytes read
 *							0 - means didn't read anything
 *		SIDE EFFECTS: none
 *		EDGE CASES: invalid inode (out of range)
 *						found an invalid data block index (out of range)
 *						offset >= file length, this accoutns for if file length = 0
 *						length = 0, Why even call this function, dummy...
 *
 */
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)
{
	// edge case checking
	uint32_t this_file_size;
	// finding where to start
	uint32_t start_block, start_offset;
	// finding where to end
	uint32_t end_search, end_block, end_offset;
	// doing the task
	uint32_t block_loop;
	uint32_t chars_read;
	uint32_t curr_byte; // keep track of current byte
	uint32_t curr_data_block; // current data block
	uint32_t end_of_block_byte;
	if(inode >= num_inodes)
		return -1; // inode number too big
	this_file_size = (inodes[inode]).file_size; // reduce syntax
	// offset is greater than file size or no need to read
	if((offset >= this_file_size) || (length == 0))
		return 0;
	/* Find the start location (data block, offset in data block) */
	start_block = offset/MAX_CHARS_IN_DATA;
	start_offset = offset%MAX_CHARS_IN_DATA; // offset in start block
	/* Find the end location (data block, offset in data block) */
	end_search = offset+length;
	// should not go past the total number of chars
	if(end_search > this_file_size)
		end_search = this_file_size;
	// last block we need
	end_block = end_search/MAX_CHARS_IN_DATA;	// 4096/4096 = 1
	end_offset = end_search%MAX_CHARS_IN_DATA; // offset in end block
	chars_read = 0;
	curr_byte = start_offset;
	end_of_block_byte = MAX_CHARS_IN_DATA;
	/* Start putting data into the buffer */
	for(block_loop = start_block; block_loop <= end_block; block_loop++)
	{
		curr_data_block = ((inodes[inode]).datablock_idx)[block_loop]; // get the index of current block from inode
		if(curr_data_block > num_data_blocks)
		{
			if(chars_read == 0)
				return -1; // invalid data block index, haven't read anything
			else
				return chars_read; // found invalid block, but read some stuff
		}
		if(block_loop == end_block)
			end_of_block_byte = end_offset; // last block, change how far to parse
		while(curr_byte < end_of_block_byte)
		{
			buf[chars_read] = ((data_blocks[curr_data_block]).data)[curr_byte]; // get char
			chars_read++; // next byte to fill
			curr_byte++; // next byte in filesys
		}
		curr_byte = 0; // reset to the beginning
	}
	return chars_read;
}
