/* lib.h - Defines for useful library functions
 * vim:ts=4 noexpandtab
 */

#ifndef _LIB_H
#define _LIB_H

#include "types.h"

#define VIDEO 0xB8000
#define SIZE_OF_VIDEO 2000 // number of chars in the video memory for screen

#define MAX_TERMINAL 3

/*******************************/
#define LOW_VGA 0x0F
#define HIGH_VGA 0x0E
#define VGA_SELECT 0x3D4
#define VGA_DATA 0x3D5
/*******************************/

volatile uint32_t curr_terminal; // needs to be in lib because it helps with lib.c cursor
volatile uint32_t old_terminal;
// volatile int32_t sched_proc;

int32_t printf(int8_t *format, ...);
void putc(uint8_t c);
int32_t puts(int8_t *s);
int8_t *itoa(uint32_t value, int8_t* buf, int32_t radix);
int8_t *strrev(int8_t* s);
uint32_t strlen(const int8_t* s);
void clear(void);

void* memset(void* s, int32_t c, uint32_t n);
void* memset_word(void* s, int32_t c, uint32_t n);
void* memset_dword(void* s, int32_t c, uint32_t n);
void* memcpy(void* dest, const void* src, uint32_t n);
void* memmove(void* dest, const void* src, uint32_t n);
int32_t strncmp(const int8_t* s1, const int8_t* s2, uint32_t n);
int8_t* strcpy(int8_t* dest, const int8_t*src);
int8_t* strncpy(int8_t* dest, const int8_t*src, uint32_t n);

/* Userspace address-check functions */
int32_t bad_userspace_addr(const void* addr, int32_t len);
int32_t safe_strncpy(int8_t* dest, const int8_t* src, int32_t n);

/*
 *	show_blue_screen
 *		Draws a blue color onto video memory, to be used for exceptions
 */
void show_blue_screen(void);

/*
 *	test_interrupts
 *		Increments video memory. To be used to test rtc
 */
void test_interrupts(void);

void scroll(void);

void backspace(void);

void update_cursor();

/*
 *	scroll
 *		Shifts video memory to simulate scrolling
 */
void scroll(void);

/*
 *	backspace
 *		Implement backspace by manipulating video memory
 */
void backspace(void);

/*
 *	update_cursor
 *		Change position of blinking cursor
 */
void update_cursor();

/*
 *	save_registers
 *		This is a macro that can be used to save all the registers before an interrupt.
 *		This is used with assembly wrapping. Should be used with interrupt handlers.
 *		pusha - pushes all the general purpose registers.
 *		Don't create a stack using ebp and esp stuff.
 */
#define save_registers()				\
do {											\
	asm volatile("pushal");				\
} while(0)

/*
 * restore_registers
 *		This is a macro that can be used to restore all the registers after an interrupt
 *		This is used with assembly wrapping. Should be used with interrupt handlers.
 *		popa - pops all the general purpose registers.
 *		leave - the function pointer still sets up the stack
 *		iret - returns from the interrupt, this isn't like a regular ret.
 *			it does all sorts of stuff and eats the contents of the stack.
 */
#define restore_registers()		\
do {										\
	asm volatile( "popal\n"			\
					"leave \n"			\
					"iret  \n");		\
} while(0)


/* Port read functions */
/* Inb reads a byte and returns its value as a zero-extended 32-bit
 * unsigned int */
static inline uint32_t inb(port)
{
	uint32_t val;
	asm volatile("xorl %0, %0\n \
			inb   (%w1), %b0"
			: "=a"(val)
			: "d"(port)
			: "memory" );
	return val;
}

/* Reads two bytes from two consecutive ports, starting at "port",
 * concatenates them little-endian style, and returns them zero-extended
 * */
static inline uint32_t inw(port)
{
	uint32_t val;
	asm volatile("xorl %0, %0\n   \
			inw   (%w1), %w0"
			: "=a"(val)
			: "d"(port)
			: "memory" );
	return val;
}

/* Reads four bytes from four consecutive ports, starting at "port",
 * concatenates them little-endian style, and returns them */
static inline uint32_t inl(port)
{
	uint32_t val;
	asm volatile("inl   (%w1), %0"
			: "=a"(val)
			: "d"(port)
			: "memory" );
	return val;
}

/* Writes a byte to a port */
#define outb(data, port)                \
do {                                    \
	asm volatile("outb  %b1, (%w0)"     \
			:                           \
			: "d" (port), "a" (data)    \
			: "memory", "cc" );         \
} while(0)

/* Writes two bytes to two consecutive ports */
#define outw(data, port)                \
do {                                    \
	asm volatile("outw  %w1, (%w0)"     \
			:                           \
			: "d" (port), "a" (data)    \
			: "memory", "cc" );         \
} while(0)

/* Writes four bytes to four consecutive ports */
#define outl(data, port)                \
do {                                    \
	asm volatile("outl  %l1, (%w0)"     \
			:                           \
			: "d" (port), "a" (data)    \
			: "memory", "cc" );         \
} while(0)

/* Clear interrupt flag - disables interrupts on this processor */
#define cli()                           \
do {                                    \
	asm volatile("cli"                  \
			:                       \
			:                       \
			: "memory", "cc"        \
			);                      \
} while(0)

/* Save flags and then clear interrupt flag
 * Saves the EFLAGS register into the variable "flags", and then
 * disables interrupts on this processor */
#define cli_and_save(flags)             \
do {                                    \
	asm volatile("pushfl        \n      \
			popl %0         \n      \
			cli"                    \
			: "=r"(flags)           \
			:                       \
			: "memory", "cc"        \
			);                      \
} while(0)

/* Set interrupt flag - enable interrupts on this processor */
#define sti()                           \
do {                                    \
	asm volatile("sti"                  \
			:                       \
			:                       \
			: "memory", "cc"        \
			);                      \
} while(0)

/* Restore flags
 * Puts the value in "flags" into the EFLAGS register.  Most often used
 * after a cli_and_save_flags(flags) */
#define restore_flags(flags)            \
do {                                    \
	asm volatile("pushl %0      \n      \
			popfl"                  \
			:                       \
			: "r"(flags)            \
			: "memory", "cc"        \
			);                      \
} while(0)

#endif /* _LIB_H */
