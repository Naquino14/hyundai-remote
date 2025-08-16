/// Provides devices and role management

#ifndef ROLES_H
#define ROLES_H

#include <stdbool.h>

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/linker/linker-defs.h>

extern const char* FOB_STR;
extern const char* TRC_STR;

#define DEF_ROLE_FOB 1
#define DEF_ROLE_TRC 2
#define DEF_ROLE_UKN 0

#define ROLE_IS_FOB (role_get() == ROLE_FOB)
#define ROLE_IS_TRC (role_get() == ROLE_TRC)

#define LED0_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

#define SW0_NODE DT_ALIAS(sw0)
static const struct gpio_dt_spec sw0 = GPIO_DT_SPEC_GET(SW0_NODE, gpios);
static struct gpio_callback sw0_cb_data;

#define LORA_NODE DT_NODELABEL(lora0)
static const struct device *lora = DEVICE_DT_GET(LORA_NODE);

#if defined(CONFIG_DEVICE_ROLE) && (CONFIG_DEVICE_ROLE == DEF_ROLE_FOB)
#define DISPLAY_NODE DT_NODELABEL(ssd1306)
static const struct device *display = DEVICE_DT_GET(DISPLAY_NODE);
#define BLIGHT_NODE NULL
static const struct gpio_dt_spec blight;

#define LORA_MAX_POW_DBM 14

#elif defined(CONFIG_DEVICE_ROLE) && (CONFIG_DEVICE_ROLE == DEF_ROLE_TRC)
#define DISPLAY_NODE DT_NODELABEL(st7735)
static const struct device *display = DEVICE_DT_GET(DISPLAY_NODE);
#define BLIGHT_NODE DT_ALIAS(blight)
static const struct gpio_dt_spec blight = GPIO_DT_SPEC_GET(BLIGHT_NODE, gpios);

#define LORA_MAX_POW_DBM 18 // up to 21±1

#endif

typedef enum {
   ROLE_FOB = DEF_ROLE_FOB,
   ROLE_TRC = DEF_ROLE_TRC,
   ROLE_UKN = DEF_ROLE_UKN
} dev_role;

inline const dev_role role_get() {
#if defined(CONFIG_DEVICE_ROLE) && (CONFIG_DEVICE_ROLE == DEF_ROLE_FOB)
   return ROLE_FOB;
#elif defined(CONFIG_DEVICE_ROLE) && (CONFIG_DEVICE_ROLE == DEF_ROLE_TRC)
   return ROLE_TRC;
#else
   return ROLE_UKN;
#endif
}

inline const char* role_tostring() {
   dev_role role = role_get();

   switch (role) {
      case ROLE_FOB:
         return FOB_STR;
      case ROLE_TRC:
         return TRC_STR;
      case ROLE_UKN:
      default:
         return "UNKNOWN";
   }
}

/**
 * Automatically get device role and configure GPIOs
 */
bool role_config();

bool device_config();

#endif // ROLES_H