/*
 * sched.c -
 *
 *
 */

#include "i8259.h"
#include "sched.h"
#include "filesystem.h"
#include "paging.h"
#include "terminal.h"
#include "idt.h"

// static int init_flag;

/* 
 *	pit_init
 *		DESCRIPTION:
 *			Set the entry for PIT, and set the time slice.
 *		INPUT: none
 *		OUTPUT: none
 *		RETURN VALUE: none
 */
void pit_init()
{
	// lock it
	uint32_t flags;
	cli_and_save(flags);

	// set the entry
	idt[PIT_VECTOR_NUM].present = 1;
	SET_IDT_ENTRY(idt[PIT_VECTOR_NUM], pit_handler_wrapper);

	// get the proper output info
	uint16_t output = MAX_PIT_FREQ/TIME_SLICE;
	uint8_t lower = output & LOW_BYTE;
	uint8_t higher = output >> 8;

	// initialize the ports
	// set the divisor to every 30
	outb(lower, CHANNEL0);
	outb(higher, CHANNEL0);

	// sched_proc = 0;
	// init_flag = 1;

	// unlock
	enable_irq(PIT_IRQ); // enable IRQ 0
	restore_flags(flags);
}

/* pit_handler
 *		DESCRIPTION:
 *			Handles the PIT interrupt, during the interrupt, it should
 *			switch the process that is running, by giving the other program a time slice
 *		INPUT: none
 *		OUTPUT: none
 *		RETURN VALUE: none
 */
void pit_handler()
{
	// int32_t p_id = current_process[sched_proc];

	// if (p_id > 7){
	// 	send_eoi(PIT_IRQ);
	// 	return;
	// }

	// if(init_flag){
	// 	init_flag = 0;
	// 	in_use[curr_terminal] = 0;
	// 	send_eoi(PIT_IRQ);
	// 	execute((uint8_t*)"shell");
	// }

	// /* store esp and ebp for old process */
	// asm volatile(
	// 	"movl %%esp, %0 \n"
	// 	: "=r" (proc_ESP[p_id])
	// );
	// asm volatile(
	// 	"movl %%ebp, %0 \n"
	// 	: "=r" (proc_EBP[p_id])
	// );

	// /* get new process */
	// sched_proc = (sched_proc+1)%3;
	// p_id = current_process[sched_proc];

	// if (p_id > 7){
	// 	send_eoi(PIT_IRQ);
	// 	return;
	// }

	// if(p_id < 0 && (curr_terminal == sched_proc)){
	// 	in_use[curr_terminal] = 0;
	// 	send_eoi(PIT_IRQ);
	// 	execute((uint8_t*)"shell");
	// }
	// else{
	// 	sched_proc = curr_terminal;
	// 	send_eoi(PIT_IRQ);
	// 	return;
	// }

	// /* read file into memory */
	// uint8_t *file_name = process_array[p_id]->comm;
	// dentry_t dentry;
	// uint32_t address = PROGRAM_START;
	// read_dentry_by_name((uint8_t*) file_name, &dentry);
	// read_data(dentry.inode_idx, 0, (uint8_t *)address, inodes[dentry.inode_idx].file_size);

	// /* set up paging */
	// add_process(p_id);

 //   /* prepare tss for context switch */
	// tss.esp0 = K_STACK_BOTTOM - PROCESS_SIZE * (p_id) - BYTE_SIZE/2;
 // 	tss.ss0 = KERNEL_DS;

	send_eoi(PIT_IRQ); // wait till the switch is done before continuing work

	// /* restore esp and ebp for return */
	// asm volatile(
	// 	"movl %0, %%esp \n"
	// 	:
	// 	: "r"(proc_ESP[p_id])
	// );
	// asm volatile(
	// 	"movl %0, %%ebp \n"
	// 	:
	// 	: "r"(proc_EBP[p_id])
	// );
}




