/* JC
 * testcases3_2.h - Declarations for the checkpoint 2 tests.
 *
 */


#ifndef _TESTCASES32_H
#define _TESTCASES32_H

#include "lib.h"
#include "filesystem.h"

#define SCREEN_CHAR 80

void collective_test32();

void find_something();
void print_file_text(int8_t* name, int8_t* buffer, int32_t nbytes);

#endif /* _TESTCASES32_H */
