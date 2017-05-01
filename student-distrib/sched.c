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

static int init_flag;
static int term2_init;
static int term3_init;

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

	sched_proc = 0;
	init_flag = 1;
	term2_init = 1;
	term3_init = 1;

	// unlock
	enable_irq(PIT_IRQ); // enable IRQ 0
}

/* pit_handler
 *		DESCRIPTION:
 *			Handles the PIT interrupt
 *			
 *
 */
void pit_handler()
{
	cli();

	if(init_flag){
		init_flag = 0;
		send_eoi(PIT_IRQ);
		execute((uint8_t*)"shell");
	}

	if((current_process[sched_proc] <0) || (current_process[sched_proc] >7)){
		send_eoi(PIT_IRQ);
		sti();
		return;
	}

	/* store esp and ebp for old process */
	asm volatile(
		"movl %%esp, %0 \n"
		: "=r" (process_array[current_process[sched_proc]]->return_esp)
	);
	asm volatile(
		"movl %%ebp, %0 \n"
		: "=r" (process_array[current_process[sched_proc]]->return_ebp)
	);

	sched_proc = (sched_proc+1)%3;

	if((curr_terminal == 1)&& (sched_proc == 1) && term2_init){
		term2_init = 0;
		in_use[1] = 0;
		current_process[curr_terminal] = 1;
		send_eoi(PIT_IRQ);
		execute((uint8_t*)"shell");
	}

	if(term3_init)
		sched_proc = (sched_proc+1)%3;

	if((curr_terminal == 2) && (sched_proc == 2) && term3_init){
		term3_init = 0;
		in_use[2] = 0;
		current_process[curr_terminal] = 2;
		send_eoi(PIT_IRQ);
		execute((uint8_t*)"shell");
	}

	while(current_process[sched_proc] <0)
		sched_proc = (sched_proc+1)%3;

	/* set up paging */
	add_process(current_process[sched_proc]);

   /* prepare tss for context switch */
	tss.esp0 = K_STACK_BOTTOM - PROCESS_SIZE * (current_process[sched_proc]) - BYTE_SIZE/2;
 	tss.ss0 = KERNEL_DS;

	send_eoi(PIT_IRQ); // wait till the switch is done before continuing work

	/* restore esp and ebp for return */
	asm volatile(
		"movl %0, %%esp \n"
		:
		: "r"(process_array[current_process[sched_proc]]->return_esp)
	);
	asm volatile(
		"movl %0, %%ebp \n"
		:
		: "r"(process_array[current_process[sched_proc]]->return_ebp)
	);

	sti();
}




