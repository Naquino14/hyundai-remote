#ifndef ROLES_H
#define ROLES_H

#include <zephyr/linker/linker-defs.h>
#include <stdbool.h>

extern const char* FOB_STR;
extern const char* TRC_STR;

typedef enum {
   ROLE_FOB,
   ROLE_TRC,
   ROLE_UKN
} dev_role;

inline const dev_role role_get() {
#if defined(CONFIG_DEVICE_ROLE) && (CONFIG_DEVICE_ROLE == 1)
   return ROLE_FOB;
#elif defined(CONFIG_DEVICE_ROLE) && (CONFIG_DEVICE_ROLE == 2)
   return ROLE_TRC;
#else
   return ROLE_UKN;
#endif
}

inline const char* role_tostring() {
   dev_role role = role_get();

   switch (role)
   {
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

#endif // ROLES_H