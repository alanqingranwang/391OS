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

#define STDIN_ 0 // STDIN is always fd = 0, read only, keyboard input
#define STDOUT_ 1 // STDOUT is always fd = 1, write only, terminal output

/* Template jump table structure for drivers */
typedef struct file_op_table_t {
	int32_t (*open)(const uint8_t*);
	int32_t (*read)(int32_t, uint8_t*, int32_t);
	int32_t (*write)(int32_t, const void*, int32_t);
	int32_t (*close)(int32_t);
} fd_op_table_t;

/* File Descriptor Structure Described in 7.2 Documentation */
typedef struct file_descriptor_t {
	fd_op_table_t* fd_jump; // points at the file type's driver function
	int32_t inode_ptr; // index to the inode for this file, -1 for non_files
	uint32_t file_position; // current reading location in file, read system call should update this.
	uint32_t flags; // among other things, marks file descriptor as in-use
	// flags = 0, not in use, flags = 1, in use
} fd_t;

/* Distinct operations */
fd_op_table_t rtc_ops_table;
fd_op_table_t filesys_ops_table;
fd_op_table_t dir_ops_table;
fd_op_table_t kybd_ops_table;
fd_op_table_t term_ops_table;

void fd_table_init(fd_t* new_table);
void fops_table_init();
void close_all_fd();

/* Helpers */
int32_t set_fd_info(int32_t index, fd_t file_info);
int32_t get_fd_index();
int32_t get_inode_ptr(int32_t index);

uint32_t get_file_position(int32_t index);
void add_offset(int32_t index, uint32_t amt);

void close_fd(int32_t index);
int32_t check_valid_fd(int32_t index);

#endif /* _FD_TABLE_H */
