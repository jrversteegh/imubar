/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

extern "C" {

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/i2c.h>

}

#include "sensors.h"
#include "errors.h"

static const struct gpio_dt_spec sw0_gpio = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);

static void button_pressed(const device *dev, gpio_callback *cb, uint32_t pins) {
  if (pins & BIT(sw0_gpio.pin)) {
    printk("Button pressed\n");
  }
}

void print_sensors() {
  auto acceleration = get_acceleration();
  printk("Acceleration: %.4f, %.4f, %.4f\n", acceleration.x, acceleration.y, acceleration.z);

  auto mag_field = get_magfield();
  printk("Mag field: %.4f, %.4f, %.4f\n", mag_field.x, mag_field.y, mag_field.z);
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

int main(void) {
  printk("IMUBar initializing...\n");
  initialize_buttons();
  initialize_sensors();


  printk("IMUBar running...\n");
  int i = 0;
  while (true) {
    if (gpio_pin_get_dt(&sw0_gpio)) {
      print_sensors();
    }

    ++i;
    k_sleep(K_SECONDS(1));
  }

  printk("IMUBar done.\n");
  return 0;
}
