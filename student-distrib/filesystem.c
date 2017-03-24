/* JC
 * filesystem.c - Contains the definitions for the files
 * tab size = 3, no space
 */

#include "filesystem.h"

static uint32_t num_entries;
static uint32_t num_inodes;
static uint32_t num_data_blocks;
/* pointers to the beginning of their respective blocks */
static boot_block_t* boot_block;
static dentry_t* entries; // points to the very first entry
static inode_t* inodes; // start of all the inodes
static data_block_t* data_blocks; // where the data blocks start
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

	fd_table_init(); // will be by itself later on?

	restore_flags(flags);
}

/*************************************************************/
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
		(fd_table[table_loop]).flags = 0; // initialize all not in use.
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
	for(table_loop = 2; table_loop < MAX_OPEN_FILES; table_loop++)
	{
		if((fd_table[table_loop]).flags == 0) // available index
			return table_loop;
	}

	return -1; // none available
}

/*************************************************************/

/* 
 * print_file_info
 *	Prints all the file information.
 *		file name
 *		file type
 *		file size (in bytes)
 */
void print_file_info()
{
	clear();
	uint32_t d_loop; // loops through all the dentry loops
	uint32_t char_loop; // go through all the characters
	for(d_loop = 0; d_loop < num_entries; d_loop++)
	{
		// print out the name
		printf("file name: "); // can't use %s, if there's no guaranteed '\0'
		for(char_loop = 0; char_loop < MAX_NAME_CHARACTERS; char_loop++)
			printf("%c", (entries[d_loop].file_name)[char_loop]);

		for(char_loop = char_loop; char_loop < NUM_SPACES; char_loop++)
			printf(" "); // add spaces to align the rest		

		printf("file type: %d ", entries[d_loop].file_type);
		printf("file size: %d\n", inodes[(entries[d_loop].inode_idx)].file_size);
	}
}

/*
 * print_file_text
 * Prints all the text from a file
 *
 */
void print_file_text(int8_t* name)
{
	clear();
	dentry_t my_dentry;
	// find the name
	if(read_dentry_by_name((uint8_t*)name, &my_dentry) == -1)
	{
		printf("Invalid Name\n");
		return;
	}

	int32_t inodeindex = my_dentry.inode_idx;
	int32_t retval, counting;
	int8_t* buffer[5000];
	uint32_t off = 0;
	while((retval = read_data(inodeindex, off, (uint8_t*)buffer, 5000)) != 0)
	{
		if(retval == -1) // something went wrong
			return;

		for(counting = 0; counting < retval; counting++)
			printf("%c", buffer[counting]);

		off += retval;
	}
}

// trying to find rtc
void find_something()
{
	/* Testing Something */
	// print_file_info();
	printf("\n\n");
	print_file_text("hello");

	// printf("%s", (entries[0]).file_name);
	printf("\nWe have %d directory entries.\n", boot_block->num_dir_entries);
	printf("We have %d inodes.\n", boot_block->N);
	printf("We have %d data blocks.\n", boot_block->D);
	/* Ending Test Something */
}

/*************************************************************/

/* JC
 * create_char_count
 * 	DESCRIPTION:
 *			Goes through all the dentry names and stores the sizes, with
 *			the size maxing at 32 chars.
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
	int32_t given_name_len = strlen((int8_t*)fname);

	if(given_name_len > MAX_NAME_CHARACTERS)
		return -1; // not valid name, too long

	// go through all the dentries
	for(dentry_loop = 0; dentry_loop < MAX_ENTRIES; dentry_loop++)
	{
		// get the size of the entry's name from the count table
		// double check this
		entry_name_len = character_count[dentry_loop];
		// no point in checking if not the same length
		if(given_name_len == entry_name_len)
		{
			// if the strings are the same, copy over data
			if(strncmp((int8_t*)fname, (entries[dentry_loop]).file_name, entry_name_len) == 0)
			{
				strncpy(dentry->file_name, (entries[dentry_loop]).file_name, character_count[dentry_loop]); // (dest, src)
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
	end_block = end_search/MAX_CHARS_IN_DATA;
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
