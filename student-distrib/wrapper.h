#ifndef WRAPPER_H
#define WRAPPER_H

extern void keyboard_handler_wrapper(void);
extern void rtc_handler_wrapper(void);
extern void pit_handler_wrapper(void);
extern void syscall_handler_wrapper(void);
extern void user_context_switch(unsigned int entry_point);
extern void sys_ret(void);
extern void sys_ret_halt(void);

#endif
