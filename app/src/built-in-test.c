#include "built-in-test.h"
#include "roles.h"

#include <zephyr/drivers/gpio.h>
#include <zephyr/linker/linker-defs.h>

static const char* HASHES = "################################";

// get led node from dt
#define LED0_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

void run_bit() {
    printk("Waking up...\n");
    k_msleep(2 * 1000);

    printk("%s\n", HASHES);
    printk("#      HYUNDAI-REMOTE BIT      #\n");
    printk("# Board: %-21s #\n", CONFIG_BOARD);
    printk("# Role: %-22s #\n", role_tostring());
    printk("%s\n", HASHES);

    static bool state = true;

    for (;;) {
        int ret = gpio_pin_toggle_dt(&led);
        int read_state = gpio_pin_get_dt(&led);
        printk("Hello World! %s\tState: %s\tRead State: %s\t", CONFIG_BOARD, state ? "ON " : "OFF", read_state == GPIO_OUTPUT_HIGH ? "HI" : "LO");
        printk("%s%d\n\n", ret == 0 ? "    OK: " : "NOT OK: ", ret);

        state = !state;

        k_msleep(500);
    }
}

void stop_bit() {

}