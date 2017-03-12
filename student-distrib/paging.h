#ifndef _PAGING_H
#define _PAGING_H

#include "lib.h"

#define PAGE_SIZE 1024
#define PAGE_ALIGN PAGE_SIZE * 4

uint32_t page_directory[PAGE_SIZE] __attribute__((aligned(PAGE_ALIGN)));
uint32_t page_table[PAGE_SIZE] __attribute__((aligned(PAGE_ALIGN)));

void paging_init();
void enablePaging();

#endif
