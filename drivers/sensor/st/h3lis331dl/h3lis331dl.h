/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_DRIVERS_SENSOR_H3LIS331DL_h3lis331dl_H_
#define ZEPHYR_DRIVERS_SENSOR_H3LIS331DL_h3lis331dl_H_

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <zephyr/types.h>

#define H3LIS331DL_REG_CHIP_ID 0x00
#define H3LIS331DL_CHIP_ID 0x03

#define H3LIS331DL_REG_BW 0x20
#if CONFIG_H3LIS331DL_BW_10HZ
#define H3LIS331DL_BW 0x00
#elif CONFIG_H3LIS331DL_BW_20HZ
#define H3LIS331DL_BW 0x01
#elif CONFIG_H3LIS331DL_BW_40HZ
#define H3LIS331DL_BW 0x02
#elif CONFIG_H3LIS331DL_BW_75HZ
#define H3LIS331DL_BW 0x03
#elif CONFIG_H3LIS331DL_BW_150HZ
#define H3LIS331DL_BW 0x04
#elif CONFIG_H3LIS331DL_BW_300HZ
#define H3LIS331DL_BW 0x05
#elif CONFIG_H3LIS331DL_BW_600HZ
#define H3LIS331DL_BW 0x06
#elif CONFIG_H3LIS331DL_BW_1200HZ
#define H3LIS331DL_BW 0x07
#elif CONFIG_H3LIS331DL_BW_HIGHPASS
#define H3LIS331DL_BW 0x08
#elif CONFIG_H3LIS331DL_BW_BANDPASS
#define H3LIS331DL_BW 0x09
#else
#error "No bandwidth filter selected"
#endif
#define H3LIS331DL_BW_SHIFT 4

/* 0% temperature compensation */
#define H3LIS331DL_TCS 0x08
#define H3LIS331DL_TCS_SHIFT 0

#define H3LIS331DL_REG_RANGE 0x35
#if CONFIG_H3LIS331DL_RANGE_1G
#define H3LIS331DL_RANGE 0x00
#define H3LIS331DL_LSB_PER_MS2 3280
#elif CONFIG_H3LIS331DL_RANGE_1_5G
#define H3LIS331DL_RANGE 0x01
#define H3LIS331DL_LSB_PER_MS2 2184
#elif CONFIG_H3LIS331DL_RANGE_2G
#define H3LIS331DL_RANGE 0x02
#define H3LIS331DL_LSB_PER_MS2 1640
#elif CONFIG_H3LIS331DL_RANGE_3G
#define H3LIS331DL_RANGE 0x03
#define H3LIS331DL_LSB_PER_MS2 1092
#elif CONFIG_H3LIS331DL_RANGE_4G
#define H3LIS331DL_RANGE 0x04
#define H3LIS331DL_LSB_PER_MS2 820
#elif CONFIG_H3LIS331DL_RANGE_8G
#define H3LIS331DL_RANGE 0x05
#define H3LIS331DL_LSB_PER_MS2 410
#elif CONFIG_H3LIS331DL_RANGE_16G
#define H3LIS331DL_RANGE 0x06
#define H3LIS331DL_LSB_PER_MS2 205
#else
#error "No range selected"
#endif
#define H3LIS331DL_RANGE_SHIFT 1

#define H3LIS331DL_REG_ACCEL_X_LSB 0x2
#define H3LIS331DL_REG_ACCEL_X_MSB 0x3
#define H3LIS331DL_REG_ACCEL_Y_LSB 0x4
#define H3LIS331DL_REG_ACCEL_Y_MSB 0x5
#define H3LIS331DL_REG_ACCEL_Z_LSB 0x6
#define H3LIS331DL_REG_ACCEL_Z_MSB 0x7
#define H3LIS331DL_REG_TEMP 0x08

struct h3lis331dl_sample {
  uint16_t x;
  uint16_t y;
  uint16_t z;
};

struct h3lis331dl_data {
  struct h3lis331dl_sample sample;
  int8_t temp;
};

struct h3lis331dl_config {
  struct i2c_dt_spec i2c;
};

#endif /* ZEPHYR_DRIVERS_SENSOR_H3LIS331DL_h3lis331dl_H_ */