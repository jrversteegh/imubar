/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_DRIVERS_SENSOR_BMA180_BMA180_H_
#define ZEPHYR_DRIVERS_SENSOR_BMA180_BMA180_H_

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <zephyr/types.h>

#define BMA180_REG_CHIP_ID 0x00
#define BMA180_CHIP_ID 0x03

#define BMA180_REG_BW 0x20
#if CONFIG_BMA180_BW_10HZ
#define BMA180_BW 0x00
#elif CONFIG_BMA180_BW_20HZ
#define BMA180_BW 0x01
#elif CONFIG_BMA180_BW_40HZ
#define BMA180_BW 0x02
#elif CONFIG_BMA180_BW_75HZ
#define BMA180_BW 0x03
#elif CONFIG_BMA180_BW_150HZ
#define BMA180_BW 0x04
#elif CONFIG_BMA180_BW_300HZ
#define BMA180_BW 0x05
#elif CONFIG_BMA180_BW_600HZ
#define BMA180_BW 0x06
#elif CONFIG_BMA180_BW_1200HZ
#define BMA180_BW 0x07
#elif CONFIG_BMA180_BW_HIGHPASS
#define BMA180_BW 0x08
#elif CONFIG_BMA180_BW_BANDPASS
#define BMA180_BW 0x09
#else
#error "No bandwidth filter selected"
#endif
#define BMA180_BW_SHIFT 4

/* 0% temperature compensation */
#define BMA180_TCS 0x08
#define BMA180_TCS_SHIFT 0

#define BMA180_REG_RANGE 0x35
#if CONFIG_BMA180_RANGE_1G
#define BMA180_RANGE 0x00
#define BMA180_LSB_PER_MS2 3280
#elif CONFIG_BMA180_RANGE_1_5G
#define BMA180_RANGE 0x01
#define BMA180_LSB_PER_MS2 2184
#elif CONFIG_BMA180_RANGE_2G
#define BMA180_RANGE 0x02
#define BMA180_LSB_PER_MS2 1640
#elif CONFIG_BMA180_RANGE_3G
#define BMA180_RANGE 0x03
#define BMA180_LSB_PER_MS2 1092
#elif CONFIG_BMA180_RANGE_4G
#define BMA180_RANGE 0x04
#define BMA180_LSB_PER_MS2 820
#elif CONFIG_BMA180_RANGE_8G
#define BMA180_RANGE 0x05
#define BMA180_LSB_PER_MS2 410
#elif CONFIG_BMA180_RANGE_16G
#define BMA180_RANGE 0x06
#define BMA180_LSB_PER_MS2 205
#else
#error "No range selected"
#endif
#define BMA180_RANGE_SHIFT 1

#define BMA180_REG_ACCEL_X_LSB 0x2
#define BMA180_REG_ACCEL_X_MSB 0x3
#define BMA180_REG_ACCEL_Y_LSB 0x4
#define BMA180_REG_ACCEL_Y_MSB 0x5
#define BMA180_REG_ACCEL_Z_LSB 0x6
#define BMA180_REG_ACCEL_Z_MSB 0x7
#define BMA180_REG_TEMP 0x08

struct bma180_sample {
  uint16_t x;
  uint16_t y;
  uint16_t z;
};

struct bma180_data {
  struct bma180_sample sample;
  int8_t temp;
};

struct bma180_config {
  struct i2c_dt_spec i2c;
};

#endif /* ZEPHYR_DRIVERS_SENSOR_BMA180_BMA180_H_ */
