#include <stdio.h>
#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>

#include "built-in-test.h"
#include "can-dev.h"
#include "roles.h"

LOG_MODULE_REGISTER(main);

int main(void) {
    bool configured = role_config();
    if (!configured) {
        LOG_ERR("ROLE CFG FAIL");
        return 1;
    }

    
    
    // run_bit();

    return 0;
}