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
#include <zephyr/kernel/thread.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(imubar);

#include "battery.h"
#include "clock.h"
#include "errors.h"
#include "gps.h"
#include "interface.h"
#include "sensors.h"
#include "storage.h"

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

void fetch_envs(auto &envs) {
  for (auto &env : envs) {
    env->fetch();
  }
}

void fetch_envs_bus0() {
  auto &envs = get_envs_bus0();
  fetch_envs(envs);
}

void fetch_envs_bus1() {
  auto &envs = get_envs_bus1();
  fetch_envs(envs);
}

void read_envs(auto &envs, bool print) {
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
}

void read_envs_bus0(bool print) {
  auto &envs = get_envs_bus0();
  read_envs(envs, print);
}

void read_envs_bus1(bool print) {
  auto &envs = get_envs_bus1();
  read_envs(envs, print);
}

void read_imus(auto &imus, bool print) {
  if (print) {
    printk("-----\n");
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
}

void read_imus_bus0(bool print) {
  auto &imus = get_imus_bus0();
  read_imus(imus, print);
}

void read_imus_bus1(bool print) {
  auto &imus = get_imus_bus1();
  read_imus(imus, print);
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

void bus0_loop(void *arg1, void *arg2, void *arg3) {
  int i = 0;
  auto time = k_uptime_get();
  int64_t sum_rem = 0;
  while (true) {
    bool on_second = i % 100 == 0;
    read_imus_bus0(on_second);
    if (on_second) {
      fetch_envs_bus0();
    }

    time += 10;
    auto rem = time - k_uptime_get();
    if (rem > 0) {
      k_sleep(K_MSEC(rem));
    }
    sum_rem += rem;
    if (i % 1000 == 0) {
      auto duty_cycle = 100 - sum_rem / 100;
      printk("Bus 0 duty cycle: %lld%%\n\n", duty_cycle);
      sum_rem = 0;
    }

    ++i;
  }
}

void bus1_loop(void *arg1, void *arg2, void *arg3) {
  int i = 0;
  auto time = k_uptime_get();
  int64_t sum_rem = 0;
  while (true) {
    bool on_second = i % 100 == 0;
    read_imus_bus1(on_second);
    if (on_second) {
      fetch_envs_bus1();
    }

    time += 10;
    auto rem = time - k_uptime_get();
    if (rem > 0) {
      k_sleep(K_MSEC(rem));
    }
    sum_rem += rem;
    if (i % 1000 == 0) {
      auto duty_cycle = 100 - sum_rem / 100;
      printk("Bus 1 duty cycle: %lld%%\n\n", duty_cycle);
      sum_rem = 0;
    }

    ++i;
  }
}

K_THREAD_DEFINE(bus0_thread, 2048, bus0_loop, NULL, NULL, NULL, -1, K_FP_REGS,
                1000);
K_THREAD_DEFINE(bus1_thread, 2048, bus1_loop, NULL, NULL, NULL, -1, K_FP_REGS,
                1500);

int main(void) {
  LOG_INF("IMUBar initializing...");
  initialize_clock();
  initialize_led();
  initialize_storage();
  initialize_gnss();
  initialize_interface();
  initialize_sensors();
  initialize_battery();

  LOG_INF("IMUBar running...");
  int i = 0;
  auto time = k_uptime_get();
  int64_t sum_rem = 0;
  char msg[16];
  while (true) {
    bool on_second = i % 100 == 0;
    if (on_second) {
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
      printk("Main thread duty cycle: %lld%%\n\n", duty_cycle);
      sum_rem = 0;
    }
    if (i % 1000 == 250) {
      char has_data = gnss::has_data() ? 'T' : 'F';
      char has_fix = gnss::has_fix() ? 'T' : 'F';
      auto pos = gnss::get_position();
      snprintf(msg, 16, "G %c%c %.2f %.2f", has_data, has_fix,
               (double)pos.lat(), (double)pos.lon());
      LOG_INF("%s", msg);
      interface_write((uint8_t *)msg, strlen(msg));
    }
    if (i % 1000 == 500) {
      auto battery_level = check_battery();
      snprintf(msg, 16, "B %.2f V", (double)battery_level);
      LOG_INF("%s", msg);
      interface_write((uint8_t *)msg, strlen(msg));
    }
    if (i % 1000 == 750) {
      auto time_str = get_time_str();
      LOG_INF("%s", time_str.c_str());
      interface_write((uint8_t *)time_str.c_str(), time_str.length());
    }

    ++i;
  }

  LOG_INF("IMUBar done.");
  return 0;
}
