/*
 * syscall.c - Contains functions for system calls.
 *
 * tab size = 3, no space
 */

#include "syscall.h"
#include "wrapper.h"
#include "filesystem.h"
#include "terminal.h"

#define STATUS_BYTEMASK    0x000000FF
#define FILE_NAME_LENGTH   32
#define BYTES_TO_READ      28
#define ENTRY_POINT_START  24
#define MAGIC_NUMBER_SIZE	4

static uint8_t magic_numbers[4] = {0x7f, 0x45, 0x4c, 0x46};
static uint32_t extended_status;
static int8_t cmd_args[MAX_TERMINAL][TERM_BUFF_SIZE];
// holds command argument, needs this because process hasn't been created when this is parsed

/* NM
 * void pc_init()
 *  DESCRIPTION: initializes process controller for all terminals and processes
 *  INPUT: None
 *  OUTPUT: None
 *  RETURN VALUE: None
 */
void pc_init(){
	uint32_t cnt;
	curr_terminal = 0; // initialize to the first terminal
	// initialize all the terminal data
	for(cnt = 0; cnt < MAX_TERMINAL; cnt++)
		current_process[cnt] = -1;

	// initialize all the processes
	for(cnt = 0; cnt < MAX_PROCESSES; cnt++)
	{
		process_array[cnt] = NULL;
		in_use[cnt] = 0;
	}
}

/*
 * int32_t halt(uint8_t status)
 * 	DESCRIPTION:
 *			Terminates a process, returning the specified value to its parent process
 *			The system call handler itself is responsible for expanding the 8-bit argument
 *			from BL into the 32-bit return value to the parent program's execute system call.
 *			Be careful not to return all 32 bits from EBX. This call should never return to
 *			the caller.
 * 	INPUT:
 *			status - contains the status upon halting.
 *		OUTPUT:
 *		RETURN VALUE: status
 *		SIDE EFFECTS:
 *
 */
int32_t halt(uint8_t status)
{
	close_all_fd(); // gotta do it before the restart

	/* if terminating current terminals original shell, restart shell */
	if(process_array[current_process[curr_terminal]]->process_id < 3){
		in_use[process_array[current_process[curr_terminal]]->process_id] = 0;
		execute((uint8_t*)"shell");
	}

	//return status
	extended_status = (STATUS_BYTEMASK & status) + exception_flag;
	exception_flag = 0;

	/* restore esp and ebp */
	asm volatile(
		"movl %0, %%esp \n"
		:
		: "r"(process_array[current_process[curr_terminal]]->current_esp)
	);

	asm volatile(
		"movl %0, %%ebp \n"
		:
		: "r"(process_array[current_process[curr_terminal]]->current_ebp)
	);

	/* revert process controller info to parent process */
	in_use[current_process[curr_terminal]] = 0;
	current_process[curr_terminal] = process_array[current_process[curr_terminal]]->parent_id;

	/* prepare paging for context switch */
	add_process(current_process[curr_terminal]);

	/* prepare tss for context switch */
	tss.esp0 = process_array[current_process[curr_terminal]]->current_esp;
	tss.ss0  = KERNEL_DS;

	/* set return value */
	asm volatile(
		"movl %0, %%eax \n"
		:
		: "r"(extended_status)
		: "%eax"
	);

	/* return */
	asm volatile("jmp execute_return");

	return 0;
}

/* JC
 * parse_cmd_args
 *		DESCRIPTION: Given a buffer, retrieve the argument after the first space.
 *		Then fill in NULL after the first command. All the arguments are placed in the cmd_args
 *		INPUT: buf, the buffer that we should extract the command, comm the user input that needs to be parsed
 *		RETURN VALUE: none
 */
void parse_cmd_args(uint8_t* buf, const uint8_t* comm)
{
	int32_t i = 0;

	// the command should start at index 0, so find the first space
	while(comm[i] != ' ' && comm[i] != '\0' && i < TERM_BUFF_SIZE)
	{
		buf[i] = comm[i]; // get the command
		i++;
	}

	buf[i] = '\0'; // terminate the command

	// i should be at the first space, no find the next non space char
	while(comm[i] == ' ' && comm[i] != '\0' && i < TERM_BUFF_SIZE)
		i++; // get to the arguments

	// get the arguments
	int32_t file_name_length = i; // 1 char too many
	int32_t arg_cnt = i; // start parsing from where the command left off
	// parse till end of character or max buffer size
	for(; comm[arg_cnt] != '\0' && arg_cnt != TERM_BUFF_SIZE; arg_cnt++) // get all the arguments
		cmd_args[curr_terminal][arg_cnt - file_name_length] = comm[arg_cnt];

	cmd_args[curr_terminal][arg_cnt - file_name_length] = '\0'; // terminate the string
}

/*
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
int32_t execute(const uint8_t* comm)
{
	if(comm == NULL) // null pointer
		return -1;

	uint8_t command[TERM_BUFF_SIZE];
	parse_cmd_args(command, comm); // split the command from the argument

	int32_t i;

	/* get the file name and arguments */
	int32_t file_name_length;
	int8_t file_name[FILE_NAME_LENGTH];

	/* parse file and extract useful data */
	for(i = 0; command[i] != ' ' && command[i] != '\0' && command[i] != '\n'; i++) {
		if(i >= FILE_NAME_LENGTH-1) return -1;
		file_name[i] = command[i];
	}

	file_name[i] = '\0';
	file_name_length = i;

	// if it returns -1, that means the file doesn't exist
	dentry_t file_dentry;
	if(read_dentry_by_name((uint8_t*)file_name, &file_dentry) == -1) // get the data
		return -1;

	if(file_dentry.file_type != 2)
		return -1; // it's not a file type

	// Read first 4 bytes to see if its an executable
	uint8_t buf[BYTES_TO_READ];
	uint32_t read_bytes = BYTES_TO_READ;

	if(read_data(file_dentry.inode_idx, 0, buf, read_bytes) == -1)
		return -1;

	// find the next unused process
	for(i = 0; i < MAX_PROCESSES; i++) {
		if(in_use[i] == 0) {
			in_use[i] = 1;
			break;
		}
	}

	if(i >= MAX_PROCESSES) {
		printf("Maximum Possible Processes. Stop and Reconsider.\n");
		return -1;  // too many processes, Piazza post @1089, shouldn't be 0
	}

	/* create pcb and initialize it */
	pcb * process_pcb = (pcb *)(K_STACK_BOTTOM - PROCESS_SIZE*(1+i));
	process_array[i] = process_pcb;
	process_pcb->process_id = i;

	if(i < 3) { // is this the first program?
		process_pcb->parent_id = -1;
	}
	else{
		process_pcb->parent_id = current_process[curr_terminal];
	}
	current_process[curr_terminal] = i;

	// copy the arguments from parsing into the pcb
	strcpy((int8_t*)process_array[current_process[curr_terminal]]->args, cmd_args[curr_terminal]);

	fd_table_init(process_pcb->fd_table); // initialize this process's fd table

	int j;
	for(j = 0; j < MAGIC_NUMBER_SIZE; j++) {
		if(buf[j] != magic_numbers[j]) {
			return -1; // not executable
		}
	}

	/* Extract entry point of task */
	uint32_t entry_point = 0;
	for(j = ENTRY_POINT_START; j < BYTES_TO_READ; j++) {
		entry_point |= (buf[j] << (BYTE_SIZE*(j-ENTRY_POINT_START)));
	}

	/* set up paging */
	add_process(process_pcb->process_id);

	/* read file into memory */
	dentry_t dentry;
	uint32_t address = PROGRAM_START;
	if(read_dentry_by_name((uint8_t*) file_name, &dentry) == -1) {
		return -1;
	}

	if(read_data(dentry.inode_idx, 0, (uint8_t *)address, inodes[dentry.inode_idx].file_size) == -1) {
		return -1;
	}

	/* prepare tss for context switch */
	tss.esp0 = K_STACK_BOTTOM - PROCESS_SIZE * (current_process[curr_terminal]) - BYTE_SIZE/2;
	tss.ss0 = KERNEL_DS;

	/* store current esp and ebp for halt */
	asm volatile(
		"movl %%esp, %0 \n"
		: "=r"(process_array[current_process[curr_terminal]]->current_esp)
	);

	asm volatile(
		"movl %%ebp, %0 \n"
		: "=r"(process_array[current_process[curr_terminal]]->current_ebp)
	);

	/* push IRET context to stack and IRET */
	user_context_switch(entry_point);

	/* used for halting */
	asm volatile ("execute_return: ");
	asm volatile ("leave");
	asm volatile ("ret");

	/* return */
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
 * 	INPUT: fd - file descriptor index
 *				 buf - buffer we are copying to
 *				 nbytes - how many bytes to read
 *		OUTPUT:
 *		RETURN VALUE: 	in general - return number of bytes read
 *							0 - if the initial file position is at or beyond the end of file (for normal files
 *								and directory)
 *							keyboard - return data from one line that has been terminated by pressing Enter,
 *										or as much as fits in the buffer from one such line. The line should include
 *										line feed character.
 *							RTC - always return 0
 *		SIDE EFFECTS:
 *
 */
int32_t read(int32_t fd, void* buf, int32_t nbytes)
{
	if(!check_valid_fd(fd))
	{
		printf("Invalid FD, sys_read\n");
		return -1; // invalid fd
	}

	if(buf == NULL)
		return -1; // invalid pointer

 	// get the function pointer to the specific file or rtc or thing
	return (((((process_array[current_process[curr_terminal]])->fd_table)[fd]).fd_jump)->read)(fd, (uint8_t*)buf, nbytes);
}

/* JC
 * int32_t write(int32_t fd, const void* buf, int32_t nbytes)
 * 	DESCRIPTION:
 *			Writes data to the terminal or to a device (RTC). In the case of the terminal, all data should be
 *			displayed to the screen immediately. In the case of the RTC, the system call should always accept only
 *			a 4-byte integer specifying the interrupt rate in Hz, and should set the rate of periodic interrupts
 *			accordingly. set_frequency function in rtc.h
 * 	INPUT: fd - file descriptor index
 *				 buf - buffer that we are writing from
 *				 nbytes - how many bytes to write
 *		OUTPUT:
 *		RETURN VALUE: Writes to regular files should always return -1 to indicate failure since the file system
 *						  is read-only.
 *						  The call returns the number of bytes written, or -1 on failure.
 *		SIDE EFFECTS:
 *
 */
int32_t write(int32_t fd, const void* buf, int32_t nbytes)
{
	if(!check_valid_fd(fd))
	{
		printf("Invalid FD, sys_write\n");
		return -1; // invalid fd
	}

	if(buf == NULL)
		return -1; // invalid pointer

 	// get the function pointer to the specific file or rtc or thing
	return (((((process_array[current_process[curr_terminal]])->fd_table)[fd]).fd_jump)->write)(fd, buf, nbytes);
}

/* JC
 * int32_t open(const uint8_t* filename)
 * 	DESCRIPTION:
 *			Provides access to file system. The call should find the directory entry corresponding to the named file,
 *			allocate an unused file descriptor, and set up any data necessary to handle the given type of file (directory,
 *			RTC device, or regular file).
 * 	INPUT: filename - file name we are trying to open
 *		OUTPUT:
 *		RETURN VALUE: -1 - if the named file doesn't exist or no desciptors are free.
 *		SIDE EFFECTS:
 *
 */
int32_t open(const uint8_t* filename)
{
	if(filename == NULL)
	{
		printf("invalid pointer, sys_open\n");
		return -1; // check pointer
	}

	dentry_t dentry; // looking for this dentry
 	if(read_dentry_by_name(filename, &dentry) == -1) // find the dentry
 		return -1; // doesn't exist

 	// go to the proper file type
 	switch(dentry.file_type)
 	{ // open shouldn't be using the fd_table til it's opened, so we use the
 		// jump table structure directly.
 		// I used the distinct jump table directly because the filename hasn't been opened
 		// in the fd_table yet, Look at the fd_table.h, we do use the fops pointer correctly
 		// in read and write
 		case 0: return (rtc_ops_table.open)(filename);
 		case 1: return (dir_ops_table.open)(filename);
 		default: return (filesys_ops_table.open)(filename);
 	}
}

/* JC
 * int32_t close(int32_t fd)
 * 	DESCRIPTION:
 *			Closes the specified file descriptor and makes it available for return from later calls to open. You should now
 *			allow the user to close the default descriptors (0 for input and 1 for output).
 * 	INPUT: fd - file descriptor we should close
 *		OUTPUT:
 *		RETURN VALUE: -1 - trying to close an invalid descriptor
 *							0 - successful closes
 *		SIDE EFFECTS:
 *
 */
int32_t close(int32_t fd)
{
	if(!check_valid_fd(fd))
	{
		printf("Invalid FD, sys_close\n");
		return -1; // invalid fd
	}

 	// get the function pointer for the unknown function
	return (((((process_array[current_process[curr_terminal]])->fd_table)[fd]).fd_jump)->close)(fd);
}

/* JC
 * int32_t getargs(uint8_t* buf, int32_t nbytes)
 * 	DESCRIPTION:
 *			Reads the program's command line arguments into a user-level buffer. Obviously these arguments must be stored
 *			as a part of the task data when a new program is loaded. Here they were merely copied into user space. The shell
 *			does not request arguments, but you should probably still initialize the shell task's argument data to the
 *			empty string.
 * 	INPUT: buf - buffer that we need to fill with arguments
 *				 nbytes - number of bytes to move over
 *		OUTPUT:
 *		RETURN VALUE: -1 - if the arguments and a terminal NULL (0-byte) do not fit in the buffer
 *		SIDE EFFECTS:
 *
 */
int32_t getargs(uint8_t* buf, int32_t nbytes)
{
	if(buf == NULL)
		return -1; // include buffer

	uint32_t clean;
	for(clean = 0; clean < nbytes; clean++)
		buf[clean] = '\0';

	uint32_t i = 0;
	while(i < nbytes && i < TERM_BUFF_SIZE)
	{ 	// copy the data over
		buf[i] = ((process_array[current_process[curr_terminal]])->args)[i];
		i++;
	}

	if(i == nbytes)
		return -1; // couldn't fit an end of line char in there

	// start from the end, replace all the spaces with '\0' until you get to
	// the first char, this is incase the user puts in random spaces at the end
	uint32_t backwards = nbytes-1;
	while(buf[backwards] == ' ' || buf[backwards] == '\0')
	{
		buf[backwards] = (uint8_t)'\0';
		backwards--;
	}

	if(buf[0] == '\0')
		return -1; // last check, if the argument is empty, then there's no args

 	return 0;
}

/* NM
 * int32_t vidmap(uint8_t** screen_start)
 * 	DESCRIPTION:
 *			Maps the text-mode video memory into user space at a pre-set virtual address. Although the address returned is
 *			always the same (see the memory map section later in the handout), it should be written into the memory
 *			location provided by the caller (which must be checked for validity).
 *			To avoid adding kernel-side exception handling for this sort of check, you can simply check whether the address
 *			falls within the address range covered by the single user-level page. Note that the video memory will require
 *			you to add another page mapping from the program, in this case a 4kB page. It is NOT ok to simply change
 *			the permissions of the video page located < 4MB and pass that address.
 * 	INPUT:
 *			screen_start - A pointer to the location that should hold the video memory mapping
 *		OUTPUT:
 *		RETURN VALUE: -1 - if the location provided is invalid
 *		SIDE EFFECTS:
 *
 */
int32_t vidmap(uint8_t** screen_start)
{
	// invalid address location
	if(screen_start == NULL)
	{
		printf("invalid pointer, vidmap\n");
		return -1;
	}

	// check if parameter is within page allocated for user program
	if(screen_start < (uint8_t**)PROGRAM_PAGE ||
		screen_start >= (uint8_t**)(PROGRAM_PAGE + USER_PAGE_SIZE)) {
		printf("pointer out of range, vidmap\n");
		return -1;
	}

	// give user virtual adress, specific to its terminal
	switch(curr_terminal)
	{
		// terminal 1
		case 0: 
			*screen_start = (uint8_t*)VIRT_VID_TERM1;
			break;
		// terminal 2
		case 1:
			*screen_start = (uint8_t*)VIRT_VID_TERM2;
			break;
		// terminal 3
		case 2:
			*screen_start = (uint8_t*)VIRT_VID_TERM3;
			break;
	}

	// if(curr_terminal == sched_proc)
		map_virt_to_phys((uint32_t)(*screen_start), USER_VIDEO_);
	// else
		// map_virt_to_phys((uint32_t)(*screen_start), BASE_VIRT1+(sched_proc))

	return (int32_t)*screen_start; // return the virtual address
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

/* Returns -1 if the passed in cmd is invalid
 */
int32_t def_cmd(void)
{
	return -1;
}
