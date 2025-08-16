#include "roles.h"

const char *FOB_STR = "FOB-COMMANDER-XMTR";
const char *TRC_STR = "TRACK-CONTROL-XPDR";

static bool init_common()
{
    printk("Waking up...\n\n");
    
    k_msleep(2 * 1000);
    
    const char *HASHES = "################################";
    printk("%s\n", HASHES);
    printk("#           CFG INIT           #\n");
    printk("%s\n", HASHES);

    if (!device_is_ready(led.port)) {
        printk("LED device is not ready\n");
        return false;
    }

    printk("LED\t\tRDY\n");
    
    if (!device_is_ready(sw0.port)) {
        printk("User switch device is not ready\n");
        return false;
    }
    printk("User switch\tRDY\n");
    
    if (!device_is_ready(lora)) {
        printk("LoRa device is not ready\n");
        return false;
    }
    printk("LoRa\t\tRDY\n");
    
    if (!device_is_ready(display)) {
        printk("Display device is not ready\n");
        return false;
    }
    printk("Display\t\tRDY\n");

    return true;
}

static bool init_fob() {
    return true;
}

static bool init_trc() {
    return true;
}

bool role_config()
{
    // config common
    bool success = init_common();
    if (!success)
    {
        printk("Role init common failed.");
        return success;
    }

    dev_role role = role_get();

    switch (role)
    {
    case ROLE_FOB:
        success = init_fob();
        break;
    case ROLE_TRC:
        success = init_trc();
        break;
    case ROLE_UKN:
    default:
        printk("Role init failed: Unknown role.");
        return false;
    }

    printk("Role %s configuration complete.\n", role_tostring());
    return true;
}
