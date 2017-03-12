#ifndef _PAGING_H
#define _PAGING_H

#include "lib.h"

uint32_t page_directory[1024] __attribute__((aligned(4096)));
uint32_t page_table[1024] __attribute__((aligned(4096)));

void paging_init();
void enablePaging();

#endif
