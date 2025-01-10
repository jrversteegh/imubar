#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>
#include <lvgl.h>

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

int main() {
  initialize_interface();
  initialize_display();

  if (!gpio_is_ready_dt(&led)) {
    LOG_ERR("Led0 not ready");
    return 0;
  }

  auto ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
  if (ret < 0) {
    LOG_ERR("Failed to configure led0");
    return 0;
  }

  //test_display();

  int i = 0;
  char const* ping = "ping";
  LOG_INF("Starting loop");
  while (true) {
    ++i;
    LOG_INF("Task handler...");
    lv_task_handler();
    LOG_INF("... done");
    if (i % 100 == 0) {
      LOG_INF("Toggle led...");
      //gpio_pin_toggle_dt(&led);
      LOG_INF("... done");
      LOG_INF("Update  backlight...");
      //update_backlight();
      LOG_INF("... done");
    }
    if (i % 1000 == 0) {
      LOG_INF("%s", ping);
      //interface_write((uint8_t*)ping, 4);
    }
    //update_screen();
    k_msleep(10);
  }
  return 0;
}
