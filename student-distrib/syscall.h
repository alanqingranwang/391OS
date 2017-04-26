#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "lib.h"
#include "paging.h"
#include "x86_desc.h"
#include "fd_table.h"
#include "exceptions.h"

/* Additional Macros */
#define MAX_PROCESSES 8
#define MAX_TERMINAL 3
#define FD_TABLE_SIZE 8
#define MAX_CHARS 128
#define BYTE_MASK	0xFF
#define FIRST_AVL_PROCESS MAX_TERMINAL

/* per process data structure */
typedef struct process_control_block {
	uint8_t		process_id;
	uint32_t    parent_id;
	uint32_t    current_esp;
	uint32_t    current_ebp;
	uint8_t     args[MAX_CHARS];
	fd_t 			fd_table[FD_TABLE_SIZE];
} pcb;

/* process controller */
typedef struct process_control {
	int total_processes; // holds total process across all 3 terminals
	int no_processes[MAX_TERMINAL]; // how many processes per terminal
	int current_process[MAX_TERMINAL]; // which process index is the current terminal at
	// total pool of functions, first three should always be the terminals
	pcb * process_array[MAX_PROCESSES]; //pcb pointers for each process
	int in_use[MAX_PROCESSES];
} process_control;

/* holds all the processing info */
process_control p_c;
volatile uint32_t curr_terminal;

/* initializes the process controller */
void pc_init();
void parse_cmd_args(uint8_t* buf, const uint8_t* comm);

/* System Call Prototypes */
int32_t halt(uint8_t status);
int32_t execute(const uint8_t* comm);
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
