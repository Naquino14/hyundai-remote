#include "roles.h"

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/display/cfb.h>

const char* FOB_STR = "FOB-COMMANDER-XMTR";
const char* TRC_STR = "TRACK-CONTROL-XPDR";

// static const char* ERROR_NOT_READY = "%s is not ready.\n";
// static const char* ERROR_CONFIGURE = "Cannot configure %s.\n";

#if defined(CONFIG_DEVICE_ROLE) && (CONFIG_DEVICE_ROLE == DEF_ROLE_FOB)
#define DISPLAY_NODE DT_NODELABEL(ssd1306)

#elif defined(CONFIG_DEVICE_ROLE) && (CONFIG_DEVICE_ROLE == DEF_ROLE_TRC)
#define DISPLAY_NODE DT_NODELABEL(st7735)
#endif

static const struct device *display = DEVICE_DT_GET(DISPLAY_NODE);

static bool init_common() {
   return true;
}

static bool init_fob() {
   // init cfb
   if (!device_is_ready(display)) {
      printk("Display device not ready\n");
      return false;
   }

   int ret = cfb_framebuffer_init(display);
   if (ret != 0) {
      printk("Display init failed: %d\n", ret);
      return false;
   }
   return true;
}

static bool init_trc() {
#if defined(CONFIG_DEVICE_ROLE) && (CONFIG_DEVICE_ROLE == DEF_ROLE_TRC)
#endif
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
