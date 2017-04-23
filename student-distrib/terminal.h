/* JC
 * terminal.h - declarations for the terminal driver
 */
#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "lib.h"
#include "fd_table.h"

#define STDOUT_FD 1 // the fd for STDOUT
#define TERM_BUFF_SIZE 128

int8_t term_sys_save_buff[TERM_BUFF_SIZE];
int8_t cmd_args[TERM_BUFF_SIZE]; // holds command argument

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
