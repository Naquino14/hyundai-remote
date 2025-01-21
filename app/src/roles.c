#include "roles.h"

#include <zephyr/drivers/gpio.h>

const char* FOB_STR = "FOB-COMMANDER-XMTR";
const char* TRC_STR = "TRACK-CONTROL-XPDR";


static const char* ERROR_NOT_READY = "%s is not ready.\n";
static const char* ERROR_CONFIGURE = "Cannot configure %s.\n";

static bool init_common() {
   // init led
   static struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

   if (!gpio_is_ready_dt(&led)) {
      printk(ERROR_NOT_READY, "led0");
      return false;
   }

   if (gpio_pin_configure_dt(&led, GPIO_OUTPUT) < 0) {
      printk(ERROR_CONFIGURE, "led0");
      return false;
   }

   return true;
}

static bool init_fob() {
   return true;
}

static bool init_trc() {
   return true;
}

bool role_config() {
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
