#ifndef _IDT_H
#define _IDT_H

#include "x86_desc.h"
#include "exceptions.h"
#include "rtc.h"
#include "lib.h"
#include "keyboard.h"

void idt_init(void);

#endif /* _IDT_H */
