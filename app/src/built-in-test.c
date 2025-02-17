#include "built-in-test.h"

#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/display.h>
#include <zephyr/linker/linker-defs.h>

#include "roles.h"

static const char* HASHES = "################################";

// get led node from dt
#define LED0_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

// get i2c node from dt
#define I2C0_NODE DT_NODELABEL(i2c0)
const struct device* i2c_dev = DEVICE_DT_GET(I2C0_NODE);

// get oled reset node from dt
#define OLEDRESET_NODE DT_ALIAS(oledreset)
const struct gpio_dt_spec oledreset = GPIO_DT_SPEC_GET(OLEDRESET_NODE, gpios);

// get oled node
const struct device* display = DEVICE_DT_GET(DT_NODELABEL(ssd1306));

#define OLED_ADDY 0x3c

void run_bit() {
    printk("Waking up...\n");
    k_msleep(1000);

    printk("%s\n", HASHES);
    printk("#      HYUNDAI-REMOTE BIT      #\n");
    printk("# Board: %-21s #\n", CONFIG_BOARD);
    printk("# Role: %-22s #\n", role_tostring());
    printk("%s\n", HASHES);

    if (!device_is_ready(i2c_dev)) {
        printk("I2C0 not ready\n");
        return;
    } else
        printk("I2C0 ready\n");

    if (!device_is_ready(oledreset.port)) {
        printk("OLED reset pin not ready!\n");
        return;
    } else
        printk("OLED reset pin ready\n");

    // printk("Configuring reset pin...\n");
    // gpio_pin_configure_dt(&oledreset, GPIO_OUTPUT | GPIO_OUTPUT_INIT_LOW);
    // k_msleep(100);

    // printk("Resetting oled...\n");
    // // reset oled by holding reset low for 100 ms
    // gpio_pin_set_dt(&oledreset, 1);
    // k_msleep(100);
    // gpio_pin_set_dt(&oledreset, 0);
    // k_msleep(100);

    // Turn on the display
    display_blanking_off(display);

    printk("SSD1306 initialized!\n");

    static bool state = true;

    for (;;) {
        int ret = gpio_pin_toggle_dt(&led);
        int read_state = gpio_pin_get_dt(&led);
        printk("Hello World! %s\tState: %s\tRead State: %s\t", CONFIG_BOARD,
               state ? "ON " : "OFF",
               read_state == GPIO_OUTPUT_HIGH ? "HI" : "LO");
        printk("%s%d\n\n", ret == 0 ? "    OK: " : "NOT OK: ", ret);

        state = !state;

        k_msleep(500);
    }
}

void stop_bit() {
}