/* JC
 * testcases3_2.h - Declarations for the checkpoint 2 tests.
 *
 */


#ifndef _TESTCASES32_H
#define _TESTCASES32_H

#include "lib.h"
#include "filesystem.h"
#include "rtc.h"
#include "terminal.h"


#define SCREEN_CHAR 80
#define HIGHEST_RATES 10
#define MAX_CHAR_LENGTH 32

// X is how long you want to wait in seconds
// speed is the frequency of the interrupts
#define WAIT_X_SECONDS(X, speed)				\
do {													\
	interrupt_count = 0;							\
	while(interrupt_count < X*speed);		\
} while(0)












void collective_test32();
void print_file_text(int8_t* name, int8_t* buffer, int32_t nbytes);
void print_freq();
void printn(int8_t* buf, int32_t nbytes);

#endif /* _TESTCASES32_H */
