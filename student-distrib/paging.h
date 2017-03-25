#ifndef _PAGING_H
#define _PAGING_H

#include "lib.h"

#define PAGE_SIZE 1024
#define PAGE_ALIGN PAGE_SIZE * 4

#define KERNEL_MEM    0x00400000  // start of kernel memory (physical and virtual)
#define RW_SET_ONLY   0x00000002  // Set bit 1, enables r/w
#define RW_P_SET      RW_SET_ONLY | 0x00000001 // Set bit 0, enables present bit
#define RW_P_SIZE_SET RW_P_SET    | 0x00000080; // Set bit 7, enables larger page size


/* page directory */
uint32_t page_directory[PAGE_SIZE] __attribute__((aligned(PAGE_ALIGN)));

/* page table for video memory */
uint32_t page_table[PAGE_SIZE] __attribute__((aligned(PAGE_ALIGN)));

/* initializes paging */
void paging_init();

/* helper function to enable paging (in-line-assembly) */
void enablePaging();

#endif
