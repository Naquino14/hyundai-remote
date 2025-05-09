#ifndef ROLES_H
#define ROLES_H

#include <zephyr/linker/linker-defs.h>
#include <stdbool.h>

extern const char* FOB_STR;
extern const char* TRC_STR;

#define DEF_ROLE_FOB 1
#define DEF_ROLE_TRC 2
#define DEF_ROLE_UKN 0

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