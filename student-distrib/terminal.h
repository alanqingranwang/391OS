/* JC
 * terminal.h - declarations for the terminal driver
 */
#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "lib.h"
#include "fd_table.h"

#define STDOUT_FD 1 // the fd for STDOUT
#define TERM_BUFF_SIZE 128
/* handles the data passed in by calling correct handler function */
int32_t terminal_driver(uint32_t cmd, op_data_t input);
/* opens the terminal file */
int32_t terminal_open();
/* closes the terminal file */
int32_t terminal_close(int32_t fd);
/* reads from the terminal file */
int32_t terminal_read(int32_t fd, int8_t* buf, int32_t nbytes);
/* writes to the terminal file */
int32_t terminal_write(int32_t fd, int8_t* buf, int32_t nbytes);

int32_t terminal_retrieve(int32_t fd, uint8_t* buf, int32_t nbytes);

#endif /* _TERMINAL_H */
