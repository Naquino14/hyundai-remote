#include "roles.h"

const char *FOB_STR = "FOB-COMMANDER-XMTR";
const char *TRC_STR = "TRACK-CONTROL-XPDR";

#define LED0_NODE DT_ALIAS(led0)
const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

#define SW0_NODE DT_ALIAS(sw0)
const struct gpio_dt_spec sw0 = GPIO_DT_SPEC_GET(SW0_NODE, gpios);

#define LORA_NODE DT_NODELABEL(lora0)
const struct device *lora = DEVICE_DT_GET(LORA_NODE);

#if defined(CONFIG_DEVICE_ROLE) && (CONFIG_DEVICE_ROLE == DEF_ROLE_FOB)
#define DISPLAY_NODE DT_NODELABEL(ssd1306)
const struct device *display = DEVICE_DT_GET(DISPLAY_NODE);
#define BLIGHT_NODE NULL
const struct gpio_dt_spec blight;

#elif defined(CONFIG_DEVICE_ROLE) && (CONFIG_DEVICE_ROLE == DEF_ROLE_TRC)
#define DISPLAY_NODE DT_NODELABEL(st7735)
const struct device *display = DEVICE_DT_GET(DISPLAY_NODE);
#define BLIGHT_NODE DT_ALIAS(blight)
const struct gpio_dt_spec blight = GPIO_DT_SPEC_GET(BLIGHT_NODE, gpios);

#endif

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
