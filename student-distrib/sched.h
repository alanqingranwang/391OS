/*
 * sched.h - Contains the declarations and macros for the scheduling files.
 *
 *
 */

#ifndef SCHED_H
#define SCHED_H

#include "lib.h"
#include "syscall.h"

#define PIT_IRQ 0
#define TIME_SLICE 30	// ms = Hz
#define MAX_PIT_FREQ 1193182
#define LOW_BYTE 0x00FF

// ports
#define CHANNEL0	0x40

void pit_init();
void pit_handler();


#endif /* SCHED_H */


