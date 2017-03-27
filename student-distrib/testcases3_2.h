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
#include "keyboard.h"

#define SCREEN_CHAR 80
#define HIGHEST_RATES 10
#define HIGHEST_FREQ 32768

// X is how long you want to wait in seconds
// speed is the frequency of the interrupts
#define WAIT_X_SECONDS(X, speed)				\
do {													\
	interrupt_count = 0;							\
	while(interrupt_count < X*speed);		\
} while(0)










int get_print_one();
void set_print_one(int change);
void test_file_data(int index);
void collective_test32();
void print_file_text(int8_t* name, int8_t* buffer, int32_t nbytes);
void print_freq();

#endif /* _TESTCASES32_H */
