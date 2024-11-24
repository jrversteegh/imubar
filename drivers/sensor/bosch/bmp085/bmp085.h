/*
 * Copyright (c) 2024 J.R. Versteegh
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_DRIVERS_SENSOR_BMP085_BMP085_H_
#define ZEPHYR_DRIVERS_SENSOR_BMP085_BMP085_H_

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>

struct bmp085_data {
  uint16_t temp;
  uint16_t pressure;
  const struct device *dev;
};

struct bmp085_config {
  struct i2c_dt_spec i2c;
};

#endif /* ZEPHYR_DRIVERS_SENSOR_BMP085_BMP085_h_ */
