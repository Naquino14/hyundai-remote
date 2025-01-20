#include <stdio.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

#include "built-in-test.h"

int main(void) {
    run_bit();

    return 0;
}