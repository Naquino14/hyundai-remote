/*
 * Copyright (c) 2024 Nate Aquino
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/init.h>


static int board_heltec_htit_tracker_init(void) {
    // not yet implemented
    return 0;
}

SYS_INIT(board_heltec_htit_tracker_init, PRE_KERNEL_2,
         CONFIG_GPIO_INIT_PRIORITY);
