#ifndef _PAGING_H
#define _PAGING_H

#include "lib.h"

/* page directory */
uint32_t page_directory[1024] __attribute__((aligned(4096)));

/* page table for video memory */
uint32_t page_table[1024] __attribute__((aligned(4096)));

/* initializes paging */
void paging_init();

/* helper function to enable paging (in-line-assembly) */
void enablePaging();

#endif
