#include "paging.h"

/* 
 * 	paging_init
 *   DESCRIPTION: Initializes Paging
 *   INPUTS: none 
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Paging is initialized
 */
void paging_init() {
    /* initialize Page directory */
    int i;
    for(i = 0; i < 1024; i++) {
        page_directory[i] = 0x00000002;  /* Only R/W is set */
    }

	/* set up memory from (4-8MB) as one 4MB page*/
    page_directory[1]  = 0x00400000;
    page_directory[1] |= 0x00000083;

	/* split memory from (0-4MB) into 4kb pages */
    page_directory[0]  = (uint32_t) page_table;
    page_directory[0] |= 0x00000003;
	
	/* initialize the video memory page table */
    for(i = 0; i < 1024; i++) {
        page_table[i] = 0x00000002; /* Only R/W is set */
    }

	/* assign video memory a page */
    page_table[VIDEO >> 12]  = VIDEO;
    page_table[VIDEO >> 12] |= 0x00000003;

	/* in line assembly for paging initialization */
    enablePaging();
}

/* 
 * enablePaging
 *   DESCRIPTION: Helper function to handle in-line-assembly for
 *				setting up paging.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: enables paging
 */
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
    :	/* No outputs */
    :	/* No inputs */
    :   "%eax" /* clobbers eax */
    );

    /* Sets PG flag */
    asm volatile(
        "movl  %%cr0, %%eax;"
        "orl  $0x80000000, %%eax;"
        "movl %%eax, %%cr0;"
    :	/* No outputs */
    :	/* No inputs */
    : "%eax" /* clobbers eax */
    );
}
