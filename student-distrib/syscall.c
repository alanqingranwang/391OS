/* JC
 * syscall.c - Contains functions for system calls.
 *		
 * tab size = 3, no space
 */

#include "syscall.h"

// note to self, I need something that's the opposite of their
// do call, in piazza.

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

/* JC
 * int32_t halt(uint8_t status)
 * 	DESCRIPTION:
 *			Termines a process, returning the specified value to its parent process
 *			The system call handler itself is responsible for expanding the 8-bit argument
 *			from BL into the 32-bit return value to the parent program's execute system call.
 *			Be careful not to return all 32 bits from EBX. This call should never return to
 *			the caller
 * 	INPUT: status - 
 *		OUTPUT: 
 *		RETURN VALUE:
 *		SIDE EFFECTS:
 *
 */
		case SYS_HALT:


			syscall_return(-1); // change if necessary

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
 *		RETURN VALUE: -1 if the command cannot be executed
 *							(ex. program doesn't exist or filename not an executable)	  		
 *		SIDE EFFECTS:
 *
 */
		case SYS_EXECUTE:


			syscall_return(-1); // change if necessary

/* JC
 * int32_t read(int32_t fd, void* buf, int32_t nbytes)
 * 	DESCRIPTION:
 * 	INPUT: fd - file descriptor
 *				 buf - 
 *				 nbytes - 
 *		OUTPUT:
 *		RETURN VALUE:
 *		SIDE EFFECTS:
 *
 */
		case SYS_READ:


			syscall_return(-1); // change if necessary

/* JC
 * int32_t write(int32_t fd, const void* buf, int32_t nbytes)
 * 	DESCRIPTION:
 * 	INPUT: fd - 
 *				 buf - 
 *				 nbytes -
 *		OUTPUT:
 *		RETURN VALUE:
 *		SIDE EFFECTS:
 *
 */
		case SYS_WRITE:


			syscall_return(-1); // change if necessary

/* JC
 * int32_t open(const uint8_t* filename)
 * 	DESCRIPTION:
 * 	INPUT: filename - 
 *		OUTPUT:
 *		RETURN VALUE:
 *		SIDE EFFECTS:
 *
 */
		case SYS_OPEN:


			syscall_return(-1); // change if necessary

/* JC
 * int32_t close(int32_t fd)
 * 	DESCRIPTION:
 * 	INPUT: fd - 
 *		OUTPUT:
 *		RETURN VALUE:
 *		SIDE EFFECTS:
 *
 */
		case SYS_CLOSE:


			syscall_return(-1); // change if necessary

/* JC
 * int32_t getargs(uint8_t** screen_start)
 * 	DESCRIPTION:
 * 	INPUT: screen_start - 
 *		OUTPUT:
 *		RETURN VALUE:
 *		SIDE EFFECTS:
 *
 */
		case SYS_GETARGS:


			syscall_return(-1); // change if necessary

/* JC
 * int32_t vidmap(uint8_t** screen_start)
 * 	DESCRIPTION:
 * 	INPUT: screen_start - 
 *		OUTPUT:
 *		RETURN VALUE:
 *		SIDE EFFECTS:
 *
 */
		case SYS_VIDMAP:


			syscall_return(-1); // change if necessary

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
		case SYS_SET_HANDLER:


			syscall_return(-1); // change if necessary

/* JC
 * int32_t sigreturn(void)
 * 	DESCRIPTION:
 * 	INPUT: none
 *		OUTPUT:
 *		RETURN VALUE:
 *		SIDE EFFECTS:
 *
 */
		case SYS_SIGRETURN:


			syscall_return(-1); // change if necessary

		default:
			syscall_return(-1); // places -1 into eax, invalud number

	}
}

