/* JC
 * syscall.c - Contains functions for system calls.
 *
 * tab size = 3, no space
 */

#include "syscall.h"
#include "wrapper.h"

#define K_STACK_BOTTOM		0x007FFFFF
#define PROGRAM_PAGE		0x08000000
#define PROGRAM_START		0x08004800
#define PROCESS_SIZE     	0x00008000

// note to self, I need something that's the opposite of their
// do call, in piazza.

/* WARNING:	Some system calls need to synchronize with interrupt handlers. For example, the read system
 *		call made on the RTC device should wait until the next RTC interrupt has occurred before it returns.
 *		Use simple volatile flag variables to do this synchronization (e.g., something like int rtc_interrupt_occurred;)
 *		when possible (try something more complicated only after everything works!), and small critical sections
 *		with cli/sti. For example, writing to the RTC should blcok interrupts to interact with the device. Writing
 *		to the terminal also probably needs to block interrupts, if only briefly, to update screen data when printing
 *		(keyboard input is also printed to the screen from the interrupt handler).
 *
 * 	RTC flag, turn flag off after change, turn flag on after interrupt.
 */

/* These variables will be overwritten each time a syscall happens.
 * They will be used in each system call.
 */
static uint32_t num;
static uint32_t param1;
static uint32_t param2;
static uint32_t param3;

static uint8_t magic_numbers[4] = {0x7f, 0x45, 0x4c, 0x46};

/* JC
 * syscall_handler
 * 	DESCRIPTION:
 *			Called when an interrupt 0x80.
 *			Takes in a system call number and does the proper task.
 *		INPUT: Parameters for each case is passed in via registers.
 *				EAX - system number, described in header file.
 *				EBX - first argument
 *				ECX - second argument
 *				EDX - third argument
 *		OUTPUT:
 *		RETURN VALUE: 0 if successful
 *						 -1 failed.
 *		SIDE EFFECTS:
 *
 */
void syscall_handler()
{
	// get the parameters from registers, and place into variables
	asm volatile(
		"pusha			\n"
		"movl %%eax, %0 \n"
		"movl %%ebx, %1 \n"
		"movl %%ecx, %2 \n"
		"movl %%edx, %3 \n"
		: "=r"(num), "=r"(param1), "=r"(param2), "=r"(param3)
	);

	uint32_t retval; // holds the return value that needs to be put into EAX upon return

	switch(num)
	{
		case SYS_HALT:
			retval = halt();
			syscall_return(retval);
			
		case SYS_EXECUTE:
			retval = execute();
			syscall_return(retval);

		case SYS_READ:
			retval = read();
			syscall_return(retval);

		case SYS_WRITE:
			retval = write();
			syscall_return(retval);

		case SYS_OPEN:
			retval = open();
			syscall_return(retval);

		case SYS_CLOSE:
			retval = close();
			syscall_return(retval);

		case SYS_GETARGS:
			retval = getargs();
			syscall_return(retval);

		case SYS_VIDMAP:
			retval = vidmap();
			syscall_return(retval);

		case SYS_SET_HANDLER:
			retval = set_handler();
			syscall_return(retval);

		case SYS_SIGRETURN:
			retval = sigreturn();
			syscall_return(retval);

		default:
			syscall_return(-1);
	};
}

/* JC
 * int32_t halt(uint8_t status)
 * 	DESCRIPTION:
 *			Terminates a process, returning the specified value to its parent process
 *			The system call handler itself is responsible for expanding the 8-bit argument
 *			from BL into the 32-bit return value to the parent program's execute system call.
 *			Be careful not to return all 32 bits from EBX. This call should never return to
 *			the caller.
 * 	INPUT: status -
 *		OUTPUT:
 *		RETURN VALUE:
 *		SIDE EFFECTS:
 *
 */
int32_t halt()
{
	/* uncomment when ready */
	// uint8_t status = param1 & BYTE_MASK; // just retrieve the lower byte, safe way vs typecast

	return -1;
}

/* JC
 * int32_t execute(const uint8_t* command)
 * 	DESCRIPTION:
 *			Attempts to load and execute a new program, handing off
 *			the processor to the new program until it terminates.
 * 	INPUT: command - space-separated sequence of words.
 *							First word is the file name of the program to be executed
 *							:Rest of the command (without leading spaces) is provided for
 *								the new program on request via the getargs system call.
 *		OUTPUT:
 *		RETURN VALUE: -1 - if the command cannot be executed
 *			(ex. program doesn't exist or filename not an executable)
 *						 256 - if program dies by an exception
 *				  0 to 255 - if program executes a halt system call. value returned is given by the
 *								 program's call to halt.
 *		SIDE EFFECTS:
 *
 */
int32_t execute(const uint8_t* command)
{
	
	/** Parse args and check file validity */
	//check command validity
	if(command == NULL) return -1;

	// get the file name and arguments
	int i;
	int file_name_length;
	int8_t file_name[32];
	int8_t args[1024];

	for(i = 0; command[i] != ' '; i++) {
		if(i >= 31) return -1;
		file_name[i] = command[i];
	}
	file_name[i] = '\0';
	file_name_length = i;

	for(; command[i] != '\0'; i++) {
		args[i-1 - file_name_length] = command[i];
	}
	args[i-1 - file_name_length] = '\0';

	// Read first 4 bytes to see if its an executable
	uint8_t buf[28];
	op_data_t file_pack;
	file_pack.filename = file_name;
	file_pack.buf = buf;
	file_pack.nbytes = 28;
	int32_t myfd = file_driver(OPEN, file_pack);

	if(myfd == -1) { // is file opened?
		return -1;
	}

	file_pack.fd = myfd;
	if(file_driver(READ, file_pack) == -1) {
		return -1;
	}
	file_driver(CLOSE, file_pack);

	int j;

	for(j = 0; j < 28; j++) {
		printf("%x ", buf[j]);
	}
	for(j = 0; j < 4; j++) {
		if(buf[j] != magic_numbers[j]) {
			return -1; // not executable
		}
	}
	
	// check if we can run another process
	//if(no_processes >= 7) return -1;
	
	// Extract entry point of task
	uint32_t entry_point = 0;
	for(j = 24; j < 28; j++) {
		entry_point |= (buf[j] << (8*(j-24)));
	}
	printf("\n");

	/** set up paging */
	add_process(PROGRAM_PAGE, PROGRAM_PAGE);

	/** load the program into memory */
	op_data_t file_load;
	file_load.filename = file_name;
	file_load.address = PROGRAM_START;
	file_driver(LOAD, file_load);

	/** Create PCB/ open FDs */
	pcb * process_pcb = (pcb *)(K_STACK_BOTTOM - PROCESS_SIZE*(1+no_processes));
	process_pcb->p_id = no_processes;
	if(no_processes == 0)
	process_pcb->parent = NULL;
	else
	process_pcb->parent = (uint8_t *)(process_pcb + PROCESS_SIZE);
	no_processes++;
	
	//open fds into file_descriptor table portion of pcb
	//(JERRY LOOK HERE !!!!)
	//we have a pcb->fd_table that needs to be initialized
	//pcb->fd_table[0] holds std_in: keyboard r/w/o/c
	//pcb->fd_table[1] holds std_out: terminal r/w/o/c
	

	/** prepare for context switch */
	//write tss esp0 and ebp0 for new k_stack process (not yet implemented)
	//tss.esp0 = process_pcb - 1;
	//tss.ebp0 = process_pcb - 1;
	
	//store esp and ebp into the PCB
	uint32_t esp;
	asm volatile(
		"movl %%esp, %0 \n"
		: "=g"(esp)
	);
	process_pcb->process_stack_last = esp;
	
	uint32_t ebp;
	asm volatile(
		"movl %%ebp, %0 \n"
		: "=g"(ebp)
	);
	process_pcb->process_stack_base = ebp;
	
	/** push IRET context to stack and IRET */
	user_context_switch(entry_point);

	/** return */
	return 0;
}

/* JC
 * int32_t read(int32_t fd, void* buf, int32_t nbytes)
 * 	DESCRIPTION:
 *			Reads data from the keyboard, a file, or device (RTC), or directory. This call returns the
 *			number of bytes read.
 *			Specifically for files, data should be read to the end of the file or the end of the buffer provided,
 *			whichever occurs first.
 *			Specifically for directories, only the filename should be provided (as much as fits, or all 32 bytes),
 *			and subsequent reads should read from successive directory entries until the last is reached, at which
 *			point read should repeatedly return 0.
 *			Specifically for RTC, always return 0, but only after an interrupt has occurred (set a flag and wait
 *			until the interrupt handler clears it, then return 0).
 *
 *			You should use a jump table referenced by the task's file array to call for a generic handler for this
 *			call into a file-type-specific function. This jump table should be inserted into the file array on the
 *			open system call (see below).
 * 	INPUT: fd - file descriptor
 *				 buf -
 *				 nbytes -
 *		OUTPUT:
 *		RETURN VALUE: 	in general - return number of bytes read
 *							0 - if the initial file position is at or beyond the end of file (for normal files
 *								and directory)
 *							keyboard - return data from one line that has been terminated by pressing Enter,
 *										or as much as fits in the buffer from one such line. The line should include
 *										line feed character.
 *							RTC - always return 0
 *
 *		SIDE EFFECTS:
 *
 */
int32_t read()
{
	/* uncomment when ready */
	// int32_t fd = (int32_t)param1;
	// void* buf = (void*)param2;
	// int32_t nbytes = (int32_t)param3;

	return -1;
}

/* JC
 * int32_t write(int32_t fd, const void* buf, int32_t nbytes)
 * 	DESCRIPTION:
 *			Writes data to the terminal or to a device (RTC). In the case of the terminal, all data should be
 *			displayed to the screen immediately. In the case of the RTC, the system call should always accept only
 *			a 4-byte integer specifying the interrupt rate in Hz, and should set the rate of periodic interrupts
 *			accordingly. set_frequency function in rtc.h
 * 	INPUT: fd -
 *				 buf -
 *				 nbytes -
 *		OUTPUT:
 *		RETURN VALUE: Writes to regular files should always return -1 to indicate failure since the file system
 *						  is read-only.
 *						  The call returns the number of bytes written, or -1 on failure.
 *		SIDE EFFECTS:
 *
 */
int32_t write()
{
	/* uncomment when ready */
	// int32_t fd = (int32_t)param1;
	// const void* buf = (void*)param2;
	// int32_t nbytes = (int32_t)param3;

	return -1;
}

/* JC
 * int32_t open(const uint8_t* filename)
 * 	DESCRIPTION:
 *			Provides access to file system. The call should find the directory entry corresponding to the named file,
 *			allocate an unused file descriptor, and set up any data necessary to handle the given type of file (directory,
 *			RTC device, or regular file).
 * 	INPUT: filename -
 *		OUTPUT:
 *		RETURN VALUE: -1 - if the named file doesn't exist or no desciptors are free.
 *		SIDE EFFECTS:
 *
 */
int32_t open()
{
	/* uncomment when ready */
	// const uint8_t* filename = (uint8_t*)param1;

	// dentry_t dentry; // looking for this dentry
	// read_dentry_by_name(filename, &dentry); // find the dentry

	// if(dentry.file_type == 0) // it's the rtc
	// {
	// 	// add rtc to the file descriptor thing
	// }



	return -1;
}

/* JC
 * int32_t close(int32_t fd)
 * 	DESCRIPTION:
 *			Closes the specified file descriptor and makes it available for return from later calls to open. You should now
 *			allow the user to close the default descriptors (0 for input and 1 for output).
 * 	INPUT: fd -
 *		OUTPUT:
 *		RETURN VALUE: -1 - trying to close an invalid descriptor
 *							0 - successful closes
 *		SIDE EFFECTS:
 *
 */
int32_t close()
{
	/* uncomment when ready */
	// int32_t fd = (int32_t)param1;

	return -1;
}

/* JC
 * int32_t getargs(uint8_t* buf, int32_t nbytes)
 * 	DESCRIPTION:
 *			Reads the program's command line arguments into a user-level buffer. Obviously these arguments must be stored
 *			as a part of the task data when a new program is loaded. Here they were merely copied into user space. The shell
 *			does not request arguments, but you should probably still initialize the shell task's argument data to the
 *			empty string.
 * 	INPUT: buf -
 *				 nbytes -
 *		OUTPUT:
 *		RETURN VALUE: -1 - if the arguments and a terminal NULL (0-byte) do not fit in the buffer
 *		SIDE EFFECTS:
 *
 */
int32_t getargs()
{
	/* uncomment when ready */
	// uint8_t* buf = (uint8_t*)param1;
	// int32_t nbytes = (int32_t)param2;

	return -1;
}

/* JC
 * int32_t vidmap(uint8_t** screen_start)
 * 	DESCRIPTION:
 *			Maps the text-mode video memory into user space at a pre-set virtual address. Although the address returned is
 *			always the same (see the memory map section later in the handout), it should be written into the memory
 *			location provided by the caller (which must be checked for validity).
 *			To avoid adding kernel-side exception handling for this sort of check, you can simply check whether the address
 *			falls within the address range covered by the single user-level page. Note that the video memory will require
 *			you to add another page mapping from the program, in this case a 4kB page. It is NOT ok to simply change
 *			the permissions of the video page located < 4MB and pass that address.
 * 	INPUT: screen_start -
 *		OUTPUT:
 *		RETURN VALUE: -1 - if the location provided is invalid
 *		SIDE EFFECTS:
 *
 */
int32_t vidmap()
{
	/* uncomment when ready */
	// uint8_t** screen_start = (uint8_t**)param1;

	return -1;
}

/* JC
 * int32_t set_handler(int32_t signum, void* handler_address)
 * 	DESCRIPTION:
 * 	INPUT: signum -
 *				 handler_address -
 *		OUTPUT:
 *		RETURN VALUE:
 *		SIDE EFFECTS:
 *
 */
int32_t set_handler()
{
	/* uncomment when ready */
	// int32_t signum = (int32_t)param1;
	// void* handler_address = (void*)handler_address;

	return -1;
}

/* JC
 * int32_t sigreturn(void)
 * 	DESCRIPTION:
 * 	INPUT: none
 *		OUTPUT:
 *		RETURN VALUE:
 *		SIDE EFFECTS:
 *
 */
int32_t sigreturn(void)
{
	return -1;
}

/* NOTE:: OBSOLETE BECAUSE WE USE WRAPPER.S NOW
void user_context_switch(uint32_t entry_point) {
	asm volatile(
		"cli              \n"
		"mov $0x2B, %%ax  \n"
		"mov %%ax, %%ds   \n"
		"mov %%ax, %%es   \n"
		"mov %%ax, %%fs   \n"
		"mov %%ax, %%gs   \n"
		"mov %%esp, %%eax \n"

		"pushl $0x2B         \n"  // USER_DS
		"pushl $0x83FFFF0    \n"  // esp
		"pushf               \n"  // flags
		"popl %%ecx          \n"
		"orl  $0x200, %%ecx  \n"
		"pushl %%ecx         \n"
		"pushl $0x23         \n"  // USER_CS
		"movl %0, %%edx      \n"  // eip
		"pushl %%edx         \n"
		"iret                \n"
		:
		: "r" (entry_point)
	);
}
*/
