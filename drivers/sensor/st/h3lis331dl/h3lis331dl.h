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

#define H3LIS331DL_REG_CHIP_ID 0x0F
#define H3LIS331DL_CHIP_ID 0x32

#define H3LIS331DL_REG_CTRL1 0x20
#if defined(CONFIG_H3LIS331DL_ODR_50HZ)
#define H3LIS331DL_ODR 0x00
#elif defined(CONFIG_H3LIS331DL_ODR_100HZ)
#define H3LIS331DL_ODR 0x01
#elif defined(CONFIG_H3LIS331DL_ODR_400HZ)
#define H3LIS331DL_ODR 0x02
#elif defined(CONFIG_H3LIS331DL_ODR_1000HZ)
#define H3LIS331DL_ODR 0x03
#else
#error "No output data rate selected"
#endif
#define H3LIS331DL_X_AXIS_ENABLE 0x01
#define H3LIS331DL_Y_AXIS_ENABLE 0x02
#define H3LIS331DL_Z_AXIS_ENABLE 0x04
#define H3LIS331DL_AXES_ENABLE                                                                 \
  (H3LIS331DL_X_AXIS_ENABLE | H3LIS331DL_Y_AXIS_ENABLE | H3LIS331DL_Z_AXIS_ENABLE)
#define H3LIS331DL_AXES_SHIFT 0
#define H3LIS331DL_ODR_SHIFT 3
#define H3LIS331DL_PM_NORMAL 0x01
#define H3LIS331DL_PM_SHIFT 5

#define H3LIS331DL_REG_CTRL2 0x21
#define H3LIS331DL_REG_CTRL3 0x22

#define H3LIS331DL_REG_CTRL4 0x23
#if defined(CONFIG_H3LIS331DL_RANGE_100G)
#define H3LIS331DL_RANGE 0x00
#define H3LIS331DL_LSB_PER_KMS2 33285
#elif defined(CONFIG_H3LIS331DL_RANGE_200G)
#define H3LIS331DL_RANGE 0x01
#define H3LIS331DL_LSB_PER_KMS2 16643
#elif defined(CONFIG_H3LIS331DL_RANGE_400G)
#define H3LIS331DL_RANGE 0x03
#define H3LIS331DL_LSB_PER_KMS2 8321
#else
#error "No g range selected"
#endif
#define H3LIS331DL_RANGE_SHIFT 4

#define H3LIS331DL_REG_CTRL5 0x24

#define H3LIS331DL_REG_ACCEL_X_LSB 0x28
#define H3LIS331DL_REG_ACCEL_X_MSB 0x29
#define H3LIS331DL_REG_ACCEL_Y_LSB 0x2A
#define H3LIS331DL_REG_ACCEL_Y_MSB 0x2B
#define H3LIS331DL_REG_ACCEL_Z_LSB 0x2C
#define H3LIS331DL_REG_ACCEL_Z_MSB 0x2D
#define H3LIS331DL_REG_TEMP 0x08

struct h3lis331dl_sample {
  uint16_t x;
  uint16_t y;
  uint16_t z;
};

struct h3lis331dl_data {
  struct h3lis331dl_sample sample;
};

struct h3lis331dl_config {
  struct i2c_dt_spec i2c;
};

#endif /* ZEPHYR_DRIVERS_SENSOR_H3LIS331DL_h3lis331dl_H_ */
