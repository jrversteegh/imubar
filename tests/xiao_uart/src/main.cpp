#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>


LOG_MODULE_REGISTER(xiao_uart_test);

#define LED0_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

int main() {
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
  char const* ping = "ping";
  LOG_INF("Starting loop");
  while (true) {
    ++i;
    if (i % 100 == 0) {
      LOG_INF("Toggle led...");
      gpio_pin_toggle_dt(&led);
      LOG_INF("... done");
    }
    k_msleep(10);
  }
  return 0;
}
