/* JC
 * filesystem.h - Contains structures' prototypes to interpret the given filesys_img
 *		Declares function prototypes to read from the filesystem.
 * tab size = 3, no space
 */

#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

#include "lib.h"
#include "syscall.h"

/* All The Macros */
/* Macro for filesystem init in entry code */
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

#define NUM_SPACES 34 // used in formating file info's print

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

/* Print File System's Info */
void print_file_info();
int32_t print_name(int8_t* buf, int32_t max_char);
uint32_t get_num_entries();
int8_t* get_entry_name(uint32_t index);

/* Initializes the file system with relevant information */
void filesystem_init(boot_block_t* boot_addr);
void create_char_count(); // helper

/* The three routines provided by the file system module return -1 on failure
 * more documentation in MP3.
 */
int32_t read_dentry_by_name(const uint8_t *fname, dentry_t *dentry);
int32_t read_dentry_by_index(uint32_t index, dentry_t *dentry);
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

/************Dir Driver Stuff**************/
int32_t dir_open(const uint8_t* filename);
int32_t dir_read(int32_t fd, uint8_t* buf, int32_t nbytes);
int32_t dir_write(int32_t fd, const void* blank1, int32_t blank2);
int32_t dir_close(int32_t fd);
/***********File Driver Stuff**************/
int32_t file_open(const uint8_t* filename);
int32_t file_read(int32_t fd, uint8_t* buf, int32_t nbytes);
int32_t file_write(int32_t fd, const void* blank1, int32_t blank2);
int32_t file_close(int32_t fd);
/******************************************/

/* pointers to the beginning of their respective blocks */
boot_block_t* boot_block;
dentry_t* entries; // points to the very first entry
inode_t* inodes; // start of all the inodes
data_block_t* data_blocks; // where the data blocks start

#endif /* _FILESYSTEM_H */
