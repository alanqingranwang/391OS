/* JC
 * terminal.h - declarations for the terminal driver
 */
#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "lib.h"

#define STDOUT_FD 1 // the fd for STDOUT
#define TERM_BUFF_SIZE 128

int32_t current_process[MAX_TERMINAL]; // which process index is the current terminal at

int32_t terminal_init();

int32_t terminal_switch(uint32_t new_terminal);
/* opens the terminal file */
int32_t terminal_open(const uint8_t* blank1);
/* reads from the terminal file */
int32_t terminal_read(int32_t fd, uint8_t* buf, int32_t nbytes);
/* writes to the terminal file */
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);
/* closes the terminal file */
int32_t terminal_close(int32_t fd);

int32_t terminal_retrieve(uint8_t* buf, int32_t nbytes);

#endif /* _TERMINAL_H */
