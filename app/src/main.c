#include <stdio.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/linker/linker-defs.h>

// get led node from dt
#define LED0_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

int main(void) {
    static bool state = true;

    // configure gpio
    if (!gpio_is_ready_dt(&led)) {
        printk("LED is not ready, dying...\n");
        return 0;
    }

    if (gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE) < 0) {
        printk("Cant configure LED, dying...\n");
        return 0;
    }

    for (;;) {
        int ret = gpio_pin_toggle_dt(&led);
        int read_state = gpio_pin_get_dt(&led);
        printk("Hello World! %s\tState: %s\tRead State: %s\t", CONFIG_BOARD, state ? "ON " : "OFF", read_state == GPIO_OUTPUT_HIGH ? "HI" : "LO");
        printk("%s%d\n\n", ret == 0 ? "    OK: " : "NOT OK: ", ret);

        state = !state;

        k_msleep(500);
    }
    return 0;
}