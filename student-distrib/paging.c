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
    for(i = 0; i < PAGE_SIZE; i++) {
        page_directory[i] = RW_SET_ONLY;  /* Only R/W is set */
    }

	/* set up memory from (4-8MB) as one 4MB page*/
    page_directory[1]  = KERNEL_MEM;
    page_directory[1] |= RW_P_SIZE_SET;

	/* split memory from (0-4MB) into 4kb pages */
    page_directory[0]  = (uint32_t) page_table;
    page_directory[0] |= RW_P_SET;

	/* initialize the video memory page table */
    for(i = 0; i < PAGE_SIZE; i++) {
        page_table[i] = RW_SET_ONLY; /* Only R/W is set */
    }

	/* assign video memory a page */
    /* Shifting 12 to get the most significant bits */
    page_table[VIDEO >> TABLE_IDX_SHIFT]  = VIDEO;
    page_table[VIDEO >> TABLE_IDX_SHIFT] |= RW_P_SET;

    map_virt_to_phys(VIRT_VID_TERM1, BACKUP_VID1);
    map_virt_to_phys(VIRT_VID_TERM2, BACKUP_VID2);
    map_virt_to_phys(VIRT_VID_TERM3, BACKUP_VID3);

	/* in line assembly for paging initialization */
    enablePaging();
}

/*
 * add_process
 *   DESCRIPTION: set up paging for a new process 
 *   OUTPUTS: none
 *   INPUTS: process_id - identification for new process
 *   RETURN VALUE: None
 *   SIDE EFFECTS: 
 */
void add_process(uint32_t process_id) {
    page_directory[PROCESS_IDX] = (USER_SPACE + process_id * PROCESS_SIZE_) | PROCESS_SET;
    flush_tlb();
}

/*
 * map_virt_to_phys
 *   DESCRIPTION: Shift virtual address to index page directory and page table. Map respective
 *      ones to the given physical address
 *   OUTPUTS: none
 *   INPUTS: virtual_address - address which user will use to change video memory
 *           MAP - the physical memory address that we want to map to
 *   RETURN VALUE: None
 *   SIDE EFFECTS: 
 */
void map_virt_to_phys(uint32_t virtual_address, uint32_t PHYS)
{
    // give a user a table
    page_directory[virtual_address>>DIR_IDX_SHIFT] = (uint32_t) user_page_table | USER_MASK; // shift to find index into page directory
    user_page_table[(virtual_address & CLEAR_DIR_IDX)>>TABLE_IDX_SHIFT] = PHYS | RW_P_SET; // map the page table
    flush_tlb();
}

/*
 * enablePaging
 *   DESCRIPTION: Helper function to handle in-line-assembly for
 *				setting up paging.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: enables paging by modifying control registers
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

void flush_tlb() {
	asm volatile(
     "mov %%cr3, %%eax;"
     "mov %%eax, %%cr3;"
     :
     :
     :"%eax"                /* clobbered register */
     );
}
