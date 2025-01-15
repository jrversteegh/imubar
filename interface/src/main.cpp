#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>

#include "display.h"
#include "errors.h"
#include "../../src/interface.h"

LOG_MODULE_REGISTER(imubar);

#define LED0_NODE DT_ALIAS(led0)

/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

void check_show_messages() {
  char msg[32];
  auto size = interface_read((uint8_t*)msg, 31);
  msg[size] = '\0';
  if (size > 0) {
    LOG_INF("Message: %s", msg);
    display_show_message(msg);
  }
}

int main() {
  interface_init();
  display_init();

  if (!gpio_is_ready_dt(&led)) {
    LOG_ERR("Led0 not ready");
    return 0;
  }

  auto ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
  if (ret < 0) {
    LOG_ERR("Failed to configure led0");
    return 0;
  }

  int i = 0;
  LOG_INF("Starting loop");
  while (true) {
    ++i;
    if (i % 10 == 0) {
      LOG_DBG("Update display...");
      check_show_messages();
      LOG_DBG("... done");
    }
    if (i % 100 == 0) {
      LOG_DBG("Toggle led...");
      gpio_pin_toggle_dt(&led);
      LOG_DBG("... done");
      LOG_DBG("Update backlight...");
      display_update_backlight();
      LOG_DBG("... done");
    }
    if (i % 1000 == 0) {
      LOG_DBG("Ping");
      interface_ping();
    }
    display_update();
    k_msleep(10);
  }
  return 0;
}
