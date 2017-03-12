#include "paging.h"

void paging_init() {
    /* set each entry to not present */
    int i;
    for(i = 0; i < PAGE_SIZE; i++) {
        page_directory[i] = 0x00000002;  /* Only R/W is set */
    }

    page_directory[1]  = 0x00400000;
    page_directory[1] |= 0x00000083;

    page_directory[0]  = (uint32_t) page_table;
    page_directory[0] |= 0x00000003;

    for(i = 0; i < PAGE_SIZE; i++) {
        page_table[i] = 0x00000002;
    }

    page_table[VIDEO >> 12]  = VIDEO;
    page_table[VIDEO >> 12] |= 0x00000003;

    enablePaging();
}

void enablePaging() {
    /* Sets cr3 to address of page directory */
    asm volatile(
        "movl %0, %%eax;"
        "movl %%eax, %%cr3"
        : /* No outputs */
        : "r" (page_directory)
    );

    /* Sets PSE and PAE flag */
    asm volatile(
        "movl %%cr4, %%eax;"
        "orl  $0x00000010, %%eax;"
        "movl %%eax, %%cr4;"
    :
    :
    :   "%eax"
    );

    /* Sets PG flag */
    asm volatile(
        "movl  %%cr0, %%eax;"
        "orl  $0x80000000, %%eax;"
        "movl %%eax, %%cr0;"
    :
    :
    : "%eax"
    );
}
