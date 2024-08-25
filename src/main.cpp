/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

extern "C" {

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
}

#include "errors.h"
#include "sensors.h"

static const struct gpio_dt_spec sw0_gpio =
    GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

static void button_pressed(const device *dev, gpio_callback *cb,
                           uint32_t pins) {
  if (pins & BIT(sw0_gpio.pin)) {
    printk("Button pressed\n");
  }
}

void get_sensors(bool print) {
  auto acceleration = get_acceleration();
  if (print)
    printk("A: %7.4f, %7.4f, %7.4f  ", acceleration.x, acceleration.y,
           acceleration.z);

  auto rotation = get_rotation();
  if (print)
    printk("R: %7.4f, %7.4f, %7.4f  ", rotation.x, rotation.y, rotation.z);

  auto mag_field = get_magfield();
  if (print)
    printk("M: %7.4f, %7.4f, %7.4f\n", mag_field.x, mag_field.y, mag_field.z);
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
    printk("GPIO not ready.\n");
  }

  int ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
  if (ret < 0) {
    printk("Failed to initialize LED.\n");
  }
}

static void toggle_led() {
  static bool led_state = true;
  int ret = gpio_pin_toggle_dt(&led);
  if (ret < 0) {
    printk("Failed to toggle LED.\n");
  }
  led_state = !led_state;
}

int main(void) {
  printk("IMUBar initializing...\n");
  initialize_led();
  initialize_buttons();
  initialize_sensors();

  printk("IMUBar running...\n");
  int i = 0;
  while (true) {
    if (gpio_pin_get_dt(&sw0_gpio)) {
    }
    get_sensors(i % 10 == 0);

    if (i % 100 == 0) {
      toggle_led();
    }

    ++i;
    k_sleep(K_MSEC(5));
  }

  printk("IMUBar done.\n");
  return 0;
}
