/* JC
 * syscall.c - Contains functions for system calls.
 *
 * tab size = 3, no space
 */

#include "syscall.h"
#include "wrapper.h"
#include "filesystem.h" // need dentry and shit

#define K_STACK_BOTTOM		0x00800000
#define PROGRAM_PAGE		0x08000000
#define PROGRAM_START		0x08048000
#define PROCESS_SIZE     	0x00002000

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

static uint8_t magic_numbers[4] = {0x7f, 0x45, 0x4c, 0x46};
static uint32_t extended_status;

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
int32_t halt(uint8_t status)
{
	if(p_c.process_array[p_c.current_process]->parent_id == -1){
		p_c.no_processes = -1;
		p_c.current_process = -1;
		p_c.process_array[0] = 0;
		p_c.in_use[0] = 0;
		execute((uint8_t*)"shell");
	}

	extended_status = (0x000000FF & status) + exception_flag;

	asm volatile(
		"movl %0, %%esp \n"
		:
		: "r"(p_c.process_array[p_c.current_process]->current_esp)
	);

	asm volatile(
		"movl %0, %%ebp \n"
		:
		: "r"(p_c.process_array[p_c.current_process]->current_ebp)
	);

	p_c.in_use[p_c.current_process] = 0;
	p_c.current_process = p_c.process_array[p_c.current_process]->parent_id;
	p_c.no_processes--;

	add_process(p_c.current_process);

	tss.esp0 = p_c.process_array[p_c.current_process]->current_esp;
	tss.ss0  = KERNEL_DS;

	asm volatile(
		"movl %0, %%eax \n"
		:
		: "r"(extended_status)
		: "%eax"
	);

	asm volatile("jmp execute_return");

	return 0;
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
	int32_t i;

	// get the file name and arguments
	int32_t file_name_length;
	int8_t file_name[32];
	int8_t args[1024];

	for(i = 0; command[i] != ' ' && command[i] != '\0'; i++) {
		if(i >= 31) return -1;
		file_name[i] = command[i];
	}
	file_name[i] = '\0';
	file_name_length = i;

	for(; command[i] != '\0'; i++) {
		args[i-1 - file_name_length] = command[i];
	}
	args[i-1 - file_name_length] = '\0';


	// if it returns -1, that means the file doesn't exist
	dentry_t file_dentry;
	if(read_dentry_by_name((uint8_t*)file_name, &file_dentry) == -1) // get the data
		return -1;

	// Read first 4 bytes to see if its an executable
	uint8_t buf[28];
	uint32_t read_bytes = 28;

	if(read_data(file_dentry.inode_idx, 0, buf, read_bytes) == -1)
		return -1;

	exception_flag = 0;
	if(p_c.no_processes >= 7) {
		return -1;  // too many processes
	}
	else {
		p_c.no_processes++;
	}

	/** Parse args and check file validity */
	//check command validity
	if(command == NULL) return -1;

	/** Create PCB/ open FDs */
	for(i = 0; i < 8; i++) {
		if(p_c.in_use[i] == 0) {
			p_c.in_use[i] = 1;
			break;
		}
	}
	int32_t current_process = i;

	pcb * process_pcb = (pcb *)(K_STACK_BOTTOM - PROCESS_SIZE*(1+current_process));
	p_c.process_array[current_process] = process_pcb;
	process_pcb->process_id = current_process;

	if(current_process == 0) { // is this the first program?
		// process_pcb->parent = NULL;
		process_pcb->parent_id = -1;
	}
	else{
		// process_pcb->parent = process_array[p_c.current_process]
		process_pcb->parent_id = p_c.current_process;
	}
	p_c.current_process = current_process;

	fd_table_init(process_pcb->fd_table);


	int j;
	for(j = 0; j < 4; j++) {
		if(buf[j] != magic_numbers[j]) {
			return -1; // not executable
		}
	}

	// Extract entry point of task
	uint32_t entry_point = 0;
	for(j = 24; j < 28; j++) {
		entry_point |= (buf[j] << (8*(j-24)));
	}

	/* set up paging */
	add_process(process_pcb->process_id);

	dentry_t dentry;
	uint32_t address = 0x08048000;
	if(read_dentry_by_name((uint8_t*) file_name, &dentry) == -1) {
		return -1;
	}

	if(read_data(dentry.inode_idx, 0, (uint8_t *)address, inodes[dentry.inode_idx].file_size) == -1) {
		return -1;;
	}

	/** prepare for context switch */
	// write tss esp0 and ebp0 for new k_stack process (not yet implemented)
	tss.esp0 = K_STACK_BOTTOM - 0x2000 * (process_pcb->process_id) - 4;
	tss.ss0 = KERNEL_DS;

	asm volatile(
		"movl %%esp, %0 \n"
		: "=r"(p_c.process_array[p_c.current_process]->current_esp)
	);

	asm volatile(
		"movl %%ebp, %0 \n"
		: "=r"(p_c.process_array[p_c.current_process]->current_ebp)
	);
	/** push IRET context to stack and IRET */
	user_context_switch(entry_point);

	asm volatile ("execute_return: ");
	asm volatile ("leave");
	asm volatile ("ret");
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
int32_t read(int32_t fd, void* buf, int32_t nbytes)
{
 	/* uncomment when ready */
 	// int32_t fd = (int32_t)param1;
 	// void* buf = (void*)param2;
 	// int32_t nbytes = (int32_t)param3;

 	op_data_t unknown_pack;
 	unknown_pack.fd = fd;
 	unknown_pack.buf = buf;
 	unknown_pack.nbytes = nbytes;

 	int32_t (*func_ptr)(uint32_t, op_data_t);
 	func_ptr = ((((p_c.process_array)[p_c.current_process])->fd_table)[fd]).file_op_table_ptr;

 	return func_ptr(READ, unknown_pack);
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
int32_t write(int32_t fd, const void* buf, int32_t nbytes)
{
 	/* uncomment when ready */
 	// int32_t fd = (int32_t)param1;
 	// const void* buf = (void*)param2;
 	// int32_t nbytes = (int32_t)param3;

 	op_data_t unknown_pack;
 	unknown_pack.fd = fd;
 	unknown_pack.buf = (void*)buf;
 	unknown_pack.nbytes = nbytes;

 	int32_t (*func_ptr)(uint32_t, op_data_t);
 	func_ptr = ((((p_c.process_array)[p_c.current_process])->fd_table)[fd]).file_op_table_ptr;

 	return func_ptr(WRITE, unknown_pack);
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
int32_t open(const uint8_t* filename)
{
 	/* uncomment when ready */
 	// const uint8_t* filename = (uint8_t*)param1;

 	dentry_t dentry; // looking for this dentry
 	if(read_dentry_by_name(filename, &dentry) == -1) // find the dentry
 		return -1; // doesn't exist

 	if(dentry.file_type == 0) // it's the rtc
 	{
 		op_data_t rtc_pack; // empty param
 		return rtc_driver(OPEN, rtc_pack);
 	}
 	else if(dentry.file_type == 1) // it's the dir
 	{
 		op_data_t dir_pack;
 		dir_pack.filename = (int8_t*)filename;
 		return dir_driver(OPEN, dir_pack);
 	}
 	else // assuming it's a file
 	{
 		op_data_t file_pack;
 		file_pack.filename = (int8_t*)filename;
 		return file_driver(OPEN, file_pack);
 	}
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
int32_t close(int32_t fd)
{
 	/* uncomment when ready */
 	// int32_t fd = (int32_t)param1;

 	op_data_t unknown_pack;
 	unknown_pack.fd = fd;

 	int32_t (*func_ptr)(uint32_t, op_data_t);
 	func_ptr = ((((p_c.process_array)[p_c.current_process])->fd_table)[fd]).file_op_table_ptr;

 	return func_ptr(CLOSE, unknown_pack);
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
int32_t getargs(uint8_t* buf, int32_t nbytes)
{
 	/* uncomment when ready */
 	// uint8_t* buf = (uint8_t*)param1;
 	// int32_t nbytes = (int32_t)param2;

 	// syscall_return_failure();
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
int32_t vidmap(uint8_t** screen_start)
{
 	/* uncomment when ready */
 	// uint8_t** screen_start = (uint8_t**)param1;

 	// syscall_return_failure();
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
int32_t set_handler(int32_t signum, void* handler_address)
{
 	/* uncomment when ready */
 	// int32_t signum = (int32_t)param1;
 	// void* handler_address = (void*)handler_address;

 	// syscall_return_failure();
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
 	// syscall_return_failure();
 	return -1;
}

int32_t def_cmd(void)
{
	return -1;
}
