/* JC
 * filesystem.h - Functionality declaration for the file system
 * tab size = 3, no space
 */

#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

#include "lib.h"

/* The file system is already created for us in the file system image.
 * 	We do not need to create the structure itself.
 */

/* We need to create, 3 functions, plus some initialization to set
 * up everything. You'll also need to define file_o/c/r/w and dir_o/c/r/w, but
 *	these will be dummy functions for checkpoint 2.
 */

/* We need to create a process control block whenever a process is started.
 * 
 *
 */

// need to figure out how to access the file system's boot block, and the file system in general

/*  */
// need a file system init here
/* should automatically open stdin and stdout, which correspond to the file
 *		descriptors 0 and 1, respectively. stdin is a read-only file which corresponds
 *		to keyboard input. stdout is a write-only location in the file array, and marking
 *		the files as in-use. For the remaining six file descriptors avaialable, an entry
 *		in the file array is dynamically associated with the file being open'd whenever
 *		the open system call is made (return -1 if the array is full).
 */


/* The three routines provided by the file system module return -1 on failure
 * more documentation in MP3.
 */
int32_t read_dentry_by_name(const uint8_t *fname, dentry_t *dentry);
int32_t read_dentry_by_index(uint32_t index, dentry_t *dentry);
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);


#endif /* _FILESYSTEM_H */

