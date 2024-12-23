#include <zephyr/sys/reboot.h>

#include "errors.h"

void halt_or_reboot() {
  k_sleep(K_SECONDS(2));
  sys_reboot(SYS_REBOOT_COLD);
}
