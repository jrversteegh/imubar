/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(imubar);

#include "errors.h"
#include "sensors.h"

static const struct gpio_dt_spec sw0_gpio =
    GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

static void button_pressed(const device *dev, gpio_callback *cb,
                           uint32_t pins) {
  if (pins & BIT(sw0_gpio.pin)) {
    LOG_INF("Button pressed");
  }
}

void fetch_envs() {
  auto &envs = get_envs();
  envs[0]->fetch();
  /*
  for (auto &env : envs) {
    env->fetch();
  }
  */
}

void read_envs(bool print) {
  auto &envs = get_envs();
  if (print) {
    printk("%8.3f\n", k_uptime_get() / 1000.0);
  }
  for (auto &env : envs) {
    auto name = env->get_name();
    double uptime = env->get_time() / 1000.0;
    auto temperature = env->get_temperature();
    auto pressure = env->get_pressure();
    if (print) {
      printk("%8.3f  ", uptime);
      printk("%20.20s  ", name.c_str());
      printk("T: %2.2f ", static_cast<double>(temperature));
      printk("P: %6.0f\n", static_cast<double>(pressure));
    }
  }
  if (print) {
    printk("%8.3f\n---\n", k_uptime_get() / 1000.0);
  }
}

void read_imus(bool print) {
  auto &imus = get_imus();
  if (print) {
    printk("%8.3f\n", k_uptime_get() / 1000.0);
  }
  for (auto &imu : imus) {
    auto name = imu->get_name();
    double uptime = imu->fetch() / 1000.0;
    auto accel = imu->get_acceleration();
    auto gyro = imu->get_rotation();
    auto magn = imu->get_magfield();
    if (print) {
      printk("%8.3f  ", uptime);
      printk("%20.20s  ", name.c_str());
      printk("A: %8.4f, %8.4f, %8.4f  ", static_cast<double>(accel.x),
             static_cast<double>(accel.y), static_cast<double>(accel.z));
      printk("R: %7.4f, %7.4f, %7.4f  ", static_cast<double>(gyro.x),
             static_cast<double>(gyro.y), static_cast<double>(gyro.z));
      printk("M: %7.4f, %7.4f, %7.4f\n", static_cast<double>(magn.x),
             static_cast<double>(magn.y), static_cast<double>(magn.z));
    }
  }
  if (print) {
    printk("%8.3f\n---\n", k_uptime_get() / 1000.0);
  }
}

static void initialize_buttons() {
  static struct gpio_callback button_cb_data;

  if (!gpio_is_ready_dt(&sw0_gpio)) {
    error(0, "Button io not ready.");
    return;
  }

  gpio_pin_configure_dt(&sw0_gpio, GPIO_INPUT);

  gpio_pin_interrupt_configure_dt(&sw0_gpio, GPIO_INT_EDGE_TO_ACTIVE);

  gpio_init_callback(&button_cb_data, button_pressed, BIT(sw0_gpio.pin));

  gpio_add_callback(sw0_gpio.port, &button_cb_data);
}

static void initialize_led() {
  if (!gpio_is_ready_dt(&led)) {
    LOG_ERR("GPIO not ready.");
  }

  int ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
  if (ret < 0) {
    LOG_ERR("Failed to initialize LED.");
  }
}

static void toggle_led() {
  static bool led_state = true;
  int ret = gpio_pin_toggle_dt(&led);
  if (ret < 0) {
    LOG_ERR("Failed to toggle LED.");
  }
  led_state = !led_state;
}

int main(void) {
  LOG_INF("IMUBar initializing...");
  initialize_led();
  initialize_buttons();
  initialize_sensors();

  LOG_INF("IMUBar running...");
  int i = 0;
  auto time = k_uptime_get();
  int64_t sum_rem = 0;
  read_envs(true);
  while (true) {
    if (gpio_pin_get_dt(&sw0_gpio)) {
      read_envs(true);
    }
    read_imus(i % 100 == 0);
    fetch_envs();

    if (i % 100 == 0) {
      toggle_led();
    }

    time += 10;
    auto rem = time - k_uptime_get();
    if (rem > 0) {
      k_sleep(K_MSEC(rem));
    }
    sum_rem += rem;
    if (i % 1000 == 0) {
      auto duty_cycle = 100 - sum_rem / 100;
      printk("Duty cycle: %lld%%\n\n", duty_cycle);
      sum_rem = 0;
    }

    ++i;
  }

  LOG_INF("IMUBar done.");
  return 0;
}
