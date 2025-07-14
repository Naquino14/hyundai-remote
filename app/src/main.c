#include <stdio.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

#include "built-in-test.h"
#include "roles.h"
#include "gps-uart.h"

int main(void) {
    bool configured = role_config();
    if (!configured)
        return 1;

        
    #if defined(CONFIG_DEVICE_ROLE) && (CONFIG_DEVICE_ROLE == DEF_ROLE_TRC)
    begin_gps_uart();
    #endif
        
    run_bit();

    return 0;
}