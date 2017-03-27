#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "lib.h"
#include "fd_table.h"

/* handles the data passed in by calling correct handler function */
int32_t terminal_dipatcher(int cmd, op_data_t input);
/* opens the terminal file */
int32_t terminal_open();
/* closes the terminal file */
int32_t terminal_close(int32_t fd);
/* reads from the terminal file */
int32_t terminal_read(int32_t fd, int8_t* buf, int32_t nbytes);
/* writes to the terminal file */
int32_t terminal_write(int32_t fd, int8_t* buf, int32_t nbytes);


#endif /* _TERMINAL_H */
