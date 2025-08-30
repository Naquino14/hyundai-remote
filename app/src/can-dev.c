// temp file before I push code from home

#include "can-dev.h"

#include "roles.h"

#define CAN_NODE DT_CHOSEN(zephyr_canbus)
static const struct device* can = DEVICE_DT_GET(CAN_NODE);

bool can_init_common() {
    if (!device_is_ready(can)) {
        printk("CAN device is not ready\n");
        return false;
    }
    printk("CAN\t\tRDY\n");

    return true;
}

bool bit_can() {
    return false;
}