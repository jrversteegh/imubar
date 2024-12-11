#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>
#include <lvgl.h>

#include "display.h"

LOG_MODULE_REGISTER(imubar_interface);

#define LED0_NODE DT_ALIAS(led0)

/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

int main() {
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

  test_display();

  int i = 0;
  while (true) {
    ++i;
    lv_task_handler();
    if (i % 100 == 0) {
      gpio_pin_toggle_dt(&led);
      set_backlight((uint8_t)(i / 10));
    }
    k_msleep(10);
  }
  return 0;
}
