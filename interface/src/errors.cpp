#include "errors.h"

void halt_or_reboot() {
  k_sleep(K_SECONDS(1000000));
}
