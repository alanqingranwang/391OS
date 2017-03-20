/* JC
 * syscall.c - Contains functions for system calls.
 *		
 * tab size = 3, no space
 */

#include "syscall.h"

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
	// do I need to do this?
	save_registers();

	uint32_t num, param1, param2, param3; // more descriptive names later
	// get the parameters from registers, and place into variables
	asm volatile(
		"movl %%eax, %0 \n"
		"movl %%ebx, %1 \n"	
		"movl %%ecx, %2 \n"
		"movl %%edx, %3 \n"
		: "=r"(num), "=r"(param1), "=r"(param2), "=r"(param3)
	);

	switch(num)
	{
		case SYS_HALT:
			syscall_return(halt(param1)); // change if necessary
			break; // just incase

		case SYS_EXECUTE:
			syscall_return(execute(param1)); // change if necessary
			break; // just incase

		case SYS_READ:
			syscall_return(read(param1, param2, param3)); // change if necessary
			break; // just incase

		case SYS_WRITE:
			syscall_return(write(param1, param2, param3)); // change if necessary
			break; // just incase

		case SYS_OPEN:
			syscall_return(open(param1)); // change if necessary
			break; // just incase

		case SYS_CLOSE:
			syscall_return(close(param1)); // change if necessary
			break; // just incase

		case SYS_GETARGS:
			syscall_return(getargs(param1, param2)); // change if necessary
			break; // just incase

		case SYS_VIDMAP:
			syscall_return(vidmap(param1)); // change if necessary
			break; // just incase

		case SYS_SET_HANDLER:
			syscall_return(set_handler(param1, param2)); // change if necessary
			break; // just incase

		case SYS_SIGRETURN:
			syscall_return(sigreturn()); // change if necessary
			break; // just incase

		default:
			syscall_return(-1); // places -1 into eax, invalud number

	}
}

/* JC
 * halt
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
	return -1;
}

/* JC
 * execute
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
	return -1;
}

/* JC
 * read
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
	return -1;
}

/* JC
 * write
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
	return -1;
}

/* JC
 * open
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
	return -1;
}

/* JC
 * close
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
	return -1;
}

/* JC
 * getargs
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
	return -1;
}

/* JC
 * vidmap
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
	return -1;
}

/* JC
 * set_handler
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
 * sigreturn
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

