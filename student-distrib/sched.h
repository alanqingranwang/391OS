/*
 * sched.h -
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

// uint32_t proc_EBP[MAX_PROCESSES];
// uint32_t proc_ESP[MAX_PROCESSES];

void pit_init();
void pit_handler();


#endif /* SCHED_H */


