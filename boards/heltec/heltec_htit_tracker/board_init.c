/*
 * Copyright (c) 2025 Nate Aquino
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>

#define VEXT_PIN DT_GPIO_PIN(DT_NODELABEL(vext), gpios)
#define LED0_NODE DT_GPIO_PIN(DT_NODELABEL(led0), gpios)

#define SW0_NODE DT_ALIAS(sw0)
static const struct gpio_dt_spec sw0 = GPIO_DT_SPEC_GET(SW0_NODE, gpios);

LOG_MODULE_REGISTER(board_init, LOG_LEVEL_INF);

static int board_heltec_htit_tracker_init(void) {
    const struct device *gpio0;
    gpio0 = DEVICE_DT_GET(DT_NODELABEL(gpio0));

    if (!device_is_ready(gpio0)) {
        LOG_ERR("GPIO0 device not ready");
        return -ENODEV;
    }

    gpio_pin_configure(gpio0, VEXT_PIN, GPIO_OUTPUT);
    gpio_pin_set(gpio0, VEXT_PIN, 1);

    gpio_pin_configure(gpio0, LED0_NODE, GPIO_OUTPUT);

    // SW0 setup
    do {
        if (!gpio_is_ready_dt(&sw0)) {
            LOG_WRN("SW0 GPIO device not ready");
            break;
        }

        int ret = gpio_pin_configure_dt(&sw0, GPIO_INPUT);
        if (ret < 0) {
            LOG_WRN("Failed to configure SW0: %d", ret);
            break;
        }

        ret = gpio_pin_interrupt_configure_dt(&sw0, GPIO_INT_EDGE_TO_ACTIVE);
        if (ret < 0) {
            LOG_WRN("Failed to configure SW0 interrupt: %d", ret);
            break;
        }
    } while (false);

    LOG_INF("Board init complete.");
    return 0;
}

SYS_INIT(board_heltec_htit_tracker_init, PRE_KERNEL_2, CONFIG_GPIO_INIT_PRIORITY);
