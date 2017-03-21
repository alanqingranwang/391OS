/* JC
 * filesystem.h - Contains the file descriptor table.
 * tab size = 3, no space
 */

#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

#include "lib.h"


/* https://web.cs.wpi.edu/~rek/DCS/D04/UnixFileSystems.html
 * We need the following structs:
 *		boot block - located in the first few sectors of a file system.
 *			The boot block contains the initial bootstrap program used
 *			to load the operating system.
 *			Typically the first sector contains a boostrap program that
 *			reads in a larger bootstrap program from the next few sectors,
 *			and so forth.
 *		super block - probably not
 * 	inode
 *		data blocks
 */


/* Create a dentry_t struct */





/* The three routines provided by the file system module return -1 on failure
 * more documentation in MP3.
 */
int32_t read_dentry_by_name(const uint8_t *fname, dentry_t *dentry);
int32_t read_dentry_by_index(uint32_t index, dentry_t *dentry);
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);


#endif /* _FILESYSTEM_H */

