#ifndef _IDT_H
#define _IDT_H

#include "x86_desc.h"
#include "exceptions.h"
#include "lib.h"
#include "rtc.h"
#include "keyboard.h"
#include "wrapper.h"
//#include "syscall.h"

#define NUM_EXCEPTIONS 32
#define IDT_SIZE       256

/* vector numbers */
#define RTC_VECTOR_NUM 	40		// 0x28
#define KBD_VECTOR_NUM 	33		// 0x21
#define PIT_VECTOR_NUM 	32		// 0x20
#define SYSCALL_VECTOR_NUM 128 // 0x80

/* Initialize the idt, including mapping all 256 entries */
void idt_init(void);

#endif /* _IDT_H */
