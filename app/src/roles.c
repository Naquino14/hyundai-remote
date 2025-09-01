#include "roles.h"

#include <zephyr/logging/log.h>

const char *FOB_STR = "FOB-COMMANDER-XMTR";
const char *TRC_STR = "TRACK-CONTROL-XPDR";

#define LED0_NODE DT_ALIAS(led0)
const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

#define SW0_NODE DT_ALIAS(sw0)
const struct gpio_dt_spec sw0 = GPIO_DT_SPEC_GET(SW0_NODE, gpios);

#define LORA_NODE DT_NODELABEL(lora0)
const struct device *lora = DEVICE_DT_GET(LORA_NODE);

#define CAN_NODE DT_CHOSEN(zephyr_canbus)
const struct device *can = DEVICE_DT_GET(CAN_NODE);

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

LOG_MODULE_REGISTER(roles, LOG_LEVEL_DBG);

static bool init_common()
{ 
    const char *HASHES = "################################";
    printk("%s\n", HASHES);
    printk("#           CFG INIT           #\n");
    printk("%s\n", HASHES);

    if (!device_is_ready(led.port)) {
        LOG_ERR("LED device is not ready");
        return false;
    }

    LOG_INF("LED\t\tRDY");
    
    if (!device_is_ready(sw0.port)) {
        LOG_ERR("User switch device is not ready");
        return false;
    }
    LOG_INF("User switch\tRDY");
    
    if (!device_is_ready(lora)) {
        LOG_ERR("LoRa device is not ready");
        return false;
    }
    LOG_INF("LoRa\t\tRDY");
    
    // skip display for now, mine is broken
    // if (!device_is_ready(display)) {
    //     LOG_ERR("Display device is not ready");
    //     return false;
    // }
    // LOG_INF("Display\t\tRDY");

    if (!device_is_ready(can)) {
        LOG_ERR("CAN device is not ready");
        return false;
    }
    LOG_INF("CAN\t\tRDY");

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
        LOG_ERR("Role init common failed.");
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
        LOG_ERR("Role init failed: Unknown role.");
        return false;
    }

    LOG_INF("Role %s configuration complete.", role_tostring());
    return true;
}
