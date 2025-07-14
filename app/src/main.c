#include <stdio.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

#include "built-in-test.h"
#include "roles.h"

int main(void) {
    bool configured = role_config();
    if (!configured)
        return 1;
    
    run_bit();

    return 0;
}