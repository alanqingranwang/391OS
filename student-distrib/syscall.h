/* JC
 * syscall.h - Contains the prototypes for the 10 system calls
 *
 * tab size = 3, no space
 */

/* Instructions - (remove/modify later)
 * 	You must support ten system calls, numbered 1 through 10. As with
 *		Linux, they are invoked using int $0x80, and using a similar calling
 *		convention. In particular, the call number is placed in EAX, the first
 *		argument in EBX, then modification by the system call to avoid leaking
 *		information into the user programs. The return value is placed in EAX
 *		if the call returns (not all do); a value of -1 indicates an error, while
 *		others indicate some form of success.
 */

#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "lib.h"
// #include "rtc.h" // need to call it's stuff
#include "filesystem.h" // need dentry and shit
#include "paging.h"
#include "x86_desc.h"
#include "fd_table.h"

/* Migrated from "../syscalls/ece391sysnum.h" */
#define SYS_HALT    1
#define SYS_EXECUTE 2
#define SYS_READ    3
#define SYS_WRITE   4
#define SYS_OPEN    5
#define SYS_CLOSE   6
#define SYS_GETARGS 7
#define SYS_VIDMAP  8
// extra credit syscalls
#define SYS_SET_HANDLER  9
#define SYS_SIGRETURN  10

/* Additional Macros */
#define BYTE_MASK	0xFF

/*
 * syscall_return
 *		Slight alteration to restore registers.
 *		Restores the registers then puts the return value into eax.
 *		Then does proper interrupt return.
 */
#define syscall_return(retval)				\
do {										\
	asm volatile( "popal		  \n"		\
				  "movl %0, %%eax \n"		\
				  "iret			  \n"       \
					: 						\
					: "r" (retval)			\
					: "%eax" ); 			\
} while(0)

/* per process data structure */
typedef struct process_control_block {
	uint8_t		process_id;
	fd_t 		fd_table[8];
	uint32_t    parent_stack_ptr;
	uint32_t	parent_ss_ptr;
} pcb;

extern struct process_control{
	int no_processes = -1;
	int current_process; // index into process_array for which process is currently running
	pcb * process_array[8];
	int in_use[8] = {0,0,0,0,0,0,0,0};
} p_c;

/* Called when an INT 0x80 is raised */
void syscall_handler();

/* System Call Prototypes */
int32_t halt();
int32_t execute();
int32_t read();
int32_t write();
int32_t open();
int32_t close();
int32_t getargs();
int32_t vidmap();
int32_t set_handler();
int32_t sigreturn();

//void user_context_switch(uint32_t entry_point);
#endif /* _SYSCALL_H */
