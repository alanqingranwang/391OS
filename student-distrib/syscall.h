#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "lib.h"
#include "rtc.h"
#include "paging.h"
#include "x86_desc.h"
#include "fd_table.h"
#include "exceptions.h"

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
#define MAX_PROCESSES 8
#define FD_TABLE_SIZE 8

/* per process data structure */
typedef struct process_control_block {
	uint8_t		process_id;
	uint32_t    parent_id;
	fd_t 		fd_table[FD_TABLE_SIZE];
	uint32_t    current_esp;
	uint32_t    current_ebp;
} pcb;

/* process controller */
typedef struct process_control {
	int no_processes;
	int current_process;
	pcb * process_array[MAX_PROCESSES]; //pcb pointers for each process
	int in_use[MAX_PROCESSES];
} process_control;

/* holds all the processing info */
process_control p_c; 

/* initializes the process controller */
void pc_init();

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
int32_t def_cmd(void);

#endif /* _SYSCALL_H */
