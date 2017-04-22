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
#include "rtc.h"
#include "paging.h"
#include "x86_desc.h"
#include "fd_table.h"
#include "exceptions.h"

#define MAX_PROCESSES 8

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

/* per process data structure */
typedef struct process_control_block {
	uint8_t		process_id;
	uint32_t    parent_id;
	fd_t 		fd_table[8];
	uint32_t    current_esp;
	uint32_t    current_ebp;
} pcb;

typedef struct process_control {
	int no_processes;
	int current_process; // index into process_array for which process is currently running
	pcb * process_array[8];
	int in_use[8];
} process_control;

process_control p_c;

/* System Call Prototypes */
int32_t halt(uint8_t status);
int32_t execute(const uint8_t* command);
int32_t read(int32_t fd, void* buf, int32_t nbytes);
int32_t write(int32_t fd, const void* buf, int32_t nbytes);
int32_t open(const uint8_t* filename);
int32_t close(int32_t fd);
int32_t getargs(uint8_t* buf, int32_t nbytes);
int32_t vidmap(uint8_t** screen_start);
int32_t set_handler(int32_t signum, void* handler_address);
int32_t sigreturn(void);
int32_t def_cmd(void); // returns -1

//void user_context_switch(uint32_t entry_point);
#endif /* _SYSCALL_H */
