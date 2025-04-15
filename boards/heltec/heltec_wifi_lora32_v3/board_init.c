/*
* Copyright (c) 2024 Nate Aquino
*
* SPDX-License-Identifier: Apache-2.0
*/

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>

#define VEXT_PIN DT_GPIO_PIN(DT_NODELABEL(vext), gpios)
#define OLED_RST DT_GPIO_PIN(DT_NODELABEL(oledrst), gpios)
#define LED DT_GPIO_PIN(DT_NODELABEL(led0), gpios)

LOG_MODULE_REGISTER(board_init, LOG_LEVEL_INF);

#include <zephyr/logging/log.h>

static int board_heltec_wifi_lora32_v3_init(void) {
    const struct device *gpio;
    const struct device *gpio1;

    gpio = DEVICE_DT_GET(DT_NODELABEL(gpio0));
    gpio1 = DEVICE_DT_GET(DT_NODELABEL(gpio1));
    if (!device_is_ready(gpio)) {
        LOG_ERR("GPIO device not ready");
        return -ENODEV;
    }

    gpio_pin_configure(gpio1, VEXT_PIN, GPIO_OUTPUT);

    gpio_pin_configure(gpio1, LED, GPIO_OUTPUT);

    gpio_pin_configure(gpio, OLED_RST, GPIO_OUTPUT);

    LOG_INF("Board init complete.");
    return 0;
}

SYS_INIT(board_heltec_wifi_lora32_v3_init, PRE_KERNEL_2, CONFIG_GPIO_INIT_PRIORITY);
