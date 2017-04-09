/* JC
 * fd_table.h
 *
 */


#ifndef _FD_TABLE_H
#define _FD_TABLE_H

#include "lib.h"

/* File Descriptor Macros */
#define MAX_OPEN_FILES 8
#define FIRST_VALID_INDEX 2
#define FD_OFF 0
#define FD_ON 1

/* device driver cmd values */
#define OPEN 1
#define READ 2
#define WRITE 3
#define CLOSE 4
#define LOAD 5

// Fill with what you need, and pass as param to driver
typedef struct op_data_t {
	int8_t *filename; // used in open
	int32_t fd; // used in read, write, close
	void* buf; // used in read and write
	uint32_t nbytes; // used in read, and
	uint32_t address; // used in load
} op_data_t;

/* File Descriptor Structure Described in 7.2 Documentation */
typedef struct file_descriptor_t {
	int32_t (*file_op_table_ptr)(uint32_t, op_data_t); // points at the file type's driver function
	int32_t inode_ptr; // index to the inode for this file, -1 for non_files
	uint32_t file_position; // current reading location in file, read system call should update this.
	uint32_t flags; // among other things, marks file descriptor as in-use
	// flags = 0, not in use, flags = 1, in use
} fd_t;

void fd_table_init();

/* Helpers */
void set_fd_info(int32_t index, fd_t file_info);
int32_t get_fd_index();
int32_t get_inode_ptr(int32_t index);

uint32_t get_file_position(int32_t index);
void add_offset(int32_t index, uint32_t amt);

void close_fd(int32_t index);

#endif /* _FD_TABLE_H */
