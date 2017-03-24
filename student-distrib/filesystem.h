/* JC
 * filesystem.h - Contains structures' prototypes to interpret the given filesys_img
 *		Declares function prototypes to read from the filesystem.
 * tab size = 3, no space
 */

#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

#include "lib.h"
#include "testcases3_2.h" // uses macros

/* TEMPORARY */
#define SCREEN_CHAR 79

void find_something();

// need to create a process thingy
/* should automatically open stdin and stdout, which correspond to the file
 *		descriptors 0 and 1, respectively. stdin is a read-only file which corresponds
 *		to keyboard input. stdout is a write-only location in the file array, and marking
 *		the files as in-use. For the remaining six file descriptors avaialable, an entry
 *		in the file array is dynamically associated with the file being open'd whenever
 *		the open system call is made (return -1 if the array is full).
 */

/* All The Macros */
#define filesys_name "/filesys_img"
#define filesys_name_length 12

/* Init Offset Macros */
#define INODE_OFFSET 1

/* Struct Macros */
#define BOOT_RESERVE_SIZE 13 // the size is in 32bit units
#define MAX_NAME_CHARACTERS 32 // maximum number of characters
#define MAX_ENTRIES 63 // maximum possible dentries
#define DENTRY_SIZE 64 // size of dentry in bytes
#define DENTRY_RESERVE_SIZE 6 
#define MAX_INODE_DATA_BLOCKS 1023 // this is how many data blocks an inode can have
#define MAX_CHARS_IN_DATA 4096 // this is how many characters exist in a data block

/* File Descriptor Macros */
#define MAX_OPEN_FILES 8

/*********************File Descriptor Stufff**************/
/* device driver cmd */
#define OPEN 1
#define READ 2
#define WRITE 3
#define CLOSE 4

// Fill with what you need, and pass as param to driver
typedef struct op_data_t {
	uint8_t *filename; // used in open
	int32_t fd; // used in read, write, close
	void* buf; // used in read and write
	int32_t nbytes; // used in read, and write
} op_data_t;

/* File Descriptor Structure Described in 7.2 Documentation */
typedef struct file_descriptor_t {
	int32_t (*file_op_table_ptr)(uint32_t, op_data_t); // points at the file type's driver function
	int32_t inode_ptr; // index to the inode for this file, -1 for non_files
	uint32_t file_position; // current reading location in file, read system call should update this.
	uint32_t flags; // among other things, marks file descriptor as in-use
	// flags = 0, not in use, flags = 1, in use
} fd_t;

/* File Descriptor Table */
fd_t fd_table[MAX_OPEN_FILES];

void fd_table_init();

/* Helpers */
int32_t get_fd_index();
/********************************************************/

/* The structures used to organize the filesys_img data */
typedef struct dentry_t {
	int8_t file_name[MAX_NAME_CHARACTERS]; // 32 chars in the file name
	uint32_t file_type;
	uint32_t inode_idx;
	uint32_t reserved[DENTRY_RESERVE_SIZE];
} dentry_t; /* represents a single directory entry's information */

typedef struct boot_block_t {
	uint32_t num_dir_entries;
	uint32_t N;
	uint32_t D;
	uint32_t reserved[BOOT_RESERVE_SIZE];
	dentry_t dir_entries[MAX_ENTRIES];
} boot_block_t; /* represents the information about the file system's information */

typedef struct inode_t {
	uint32_t file_size; // measured in Bytes, can be thought of as number of chars
	int32_t datablock_idx[MAX_INODE_DATA_BLOCKS]; // holds indexs for each data block
} inode_t; /* represents where the file's data is all located */

/* data blocks should be all chars */
typedef struct data_block_t {
	int8_t data[MAX_CHARS_IN_DATA];
} data_block_t; /* Represents part of a file's set of data */


/* Initializes the file system with relevant information */
void filesystem_init(boot_block_t* boot_addr);

/* Helpers */
void create_char_count();

/* The three routines provided by the file system module return -1 on failure
 * more documentation in MP3.
 */
int32_t read_dentry_by_name(const uint8_t *fname, dentry_t *dentry);
int32_t read_dentry_by_index(uint32_t index, dentry_t *dentry);
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

/***********File Driver Stuff**************/
int32_t file_driver(uint32_t cmd, op_data_t operation_data);
int32_t file_open(const int32_t* filename);
int32_t file_read(int32_t fd, void* buf, int32_t nbytes);
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t file_close(int32_t fd);
/******************************************/

#endif /* _FILESYSTEM_H */

