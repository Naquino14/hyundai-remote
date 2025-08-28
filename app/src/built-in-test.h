#ifndef BUILT_IN_TEST_H
#define BUILT_IN_TEST_H

#include <stdbool.h>

bool bit_led();
bool bit_display(bool blocking);

bool bit_basic();

void run_bit();

void stop_bit();

#endif