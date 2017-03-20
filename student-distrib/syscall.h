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

/*
 * syscall_return
 *		Slight alteration to restore registers.
 *		Restores the registers then puts the return value into eax.
 *		Then does proper interrupt return.
 */
#define syscall_return(retval)				\
do {													\
	asm volatile( "popal\n"						\
					"movl %0, %%eax \n"			\
					"leave \n"						\
					"iret  \n"						\
					: 									\
					: "r" (retval)					\
					: "%eax" ); 					\
} while(0)


void syscall_handler();

/* Prototypes appear below. Unless otherwise specified, successful calls should
 *	return 0, and failed calls should return -1.
 */

#endif /* _SYSCALL_H */
