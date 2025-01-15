#include <zephyr/device.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "battery.h"
#include "errors.h"
#include "types.h"

LOG_MODULE_DECLARE(imubar);

#define BATTERY DT_NODELABEL(battery)
#define OFF_SWITCH DT_NODELABEL(off_switch)

#if !DT_NODE_EXISTS(BATTERY)
#error "Battery voltage gpio input not properly defined."
#endif

#if !DT_NODE_EXISTS(OFF_SWITCH)
#error "Power off switch gpio output not properly defined."
#endif

static constexpr float battery_level_multiplier = 0.002025;
static const struct adc_dt_spec battery = ADC_DT_SPEC_GET(BATTERY);
static const struct gpio_dt_spec off_switch = GPIO_DT_SPEC_GET(OFF_SWITCH, gpios);

static float get_battery_level() {
  uint16_t sample;
  struct adc_sequence sequence = {
      .options = nullptr,
      .buffer = &sample,
      .buffer_size = sizeof(sample),
      .calibrate = false,
  };
  adc_sequence_init_dt(&battery, &sequence);
  int err = adc_read_dt(&battery, &sequence);
  if (err < 0) {
    LOG_ERR("Could not read: %d", err);
    return 0.0f;
  }
  int32_t val = static_cast<int32_t>(sample);
  err = adc_raw_to_millivolts_dt(&battery, &val);
  if (err < 0) {
    LOG_ERR("Failed to convert to millivolts: %d", err);
    return 0.0f;
  }
  float value = battery_level_multiplier * static_cast<float>(val);
  static float mean = value;
  mean = 0.97f * mean + 0.03f * value;
  return mean;
}

static void switch_off() {
  LOG_INF("Enabling output...");
  int ret = gpio_pin_configure_dt(&off_switch, GPIO_OUTPUT);
  if (ret < 0) {
    LOG_ERR("Failed to configure offswitch to output");
    return;
  }
  LOG_INF("Switching off...");
  ret = gpio_pin_set_dt(&off_switch, GPIO_OUTPUT_ACTIVE);
  if (ret < 0) {
    LOG_ERR("Failed to switch off");
    return;
  }
  LOG_INF("Switched off!");
  k_msleep(1000);
  ret = gpio_pin_configure_dt(&off_switch, GPIO_DISCONNECTED);
  if (ret < 0) {
    LOG_ERR("Failed to release output");
    return;
  }
  k_msleep(1000);
  LOG_INF("Switch off ineffective...");
}

float check_battery() {
  auto battery_level = get_battery_level();
  if (battery_level < 3.45f) {
    LOG_WRN("%.2f V. Low Battery!", (double)battery_level);
    k_msleep(500);
    for (int i = 0; i < 3; ++i)
      switch_off();
    LOG_INF("Halting system...");
    k_fatal_halt(0);
  }
  return battery_level;
}

void initialize_battery() {
  if (!adc_is_ready_dt(&battery)) {
    error(2, "ADC device for battery level not ready");
  }

  int ret = adc_channel_setup_dt(&battery);
  if (ret) {
    error(2, "ADC channel setup for battery level failed: %d", ret);
  }

  // Initialize adc and mean value for battery voltage
  for (int i = 0; i < 100; ++i) {
    get_battery_level();
    k_msleep(5);
  }

  if (!gpio_is_ready_dt(&off_switch)) {
    error(2, "GPIO not ready");
  }

  ret = gpio_pin_configure_dt(&off_switch, GPIO_DISCONNECTED);
  if (ret < 0) {
    error(2, "Failed to configure offswitch to disconnected");
  }
}
