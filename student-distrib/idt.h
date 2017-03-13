#ifndef _IDT_H
#define _IDT_H

#include "x86_desc.h"
#include "exceptions.h"
#include "rtc.h"
#include "lib.h"
#include "keyboard.h"

#define NUM_EXCEPTIONS 32
#define IDT_SIZE       256

/* vector numbers */
#define RTC_VECTOR_NUM 	40		// 0x28
#define KBD_VECTOR_NUM 	33		// 0x21

/* Initialize the idt, including mapping all 256 entries */
void idt_init(void);

#endif /* _IDT_H */
