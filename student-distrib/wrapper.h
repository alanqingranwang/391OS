#ifndef WRAPPER_H
#define WRAPPER_H

extern void keyboard_handler_wrapper(void);
extern void rtc_handler_wrapper(void);
extern void syscall_handler_wrapper(void);
extern void user_context_switch(unsigned int entry_point);

#endif
