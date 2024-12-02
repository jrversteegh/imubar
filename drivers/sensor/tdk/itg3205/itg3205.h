/*
 * Copyright (c) 2024 J.R. Versteegh
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_DRIVERS_SENSOR_ITG3205_ITG3205_H_
#define ZEPHYR_DRIVERS_SENSOR_ITG3205_ITG3205_H_

#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <zephyr/types.h>

#define ITG3205_REG_CHIP_ID 0x00
#define ITG3205_CHIP_ID 0x68

#define ITG3205_REG_SR_DIVIDER 0x15
#define ITG3205_REG_BW 0x16
#define ITG3205_BW_SHIFT 1

#if defined(CONFIG_ITG3205_BW_5HZ)
#define ITG3205_BW 0x06
#elif defined(CONFIG_ITG3205_BW_10HZ)
#define ITG3205_BW 0x05
#elif defined(CONFIG_ITG3205_BW_20HZ)
#define ITG3205_BW 0x04
#elif defined(CONFIG_ITG3205_BW_42HZ)
#define ITG3205_BW 0x03
#elif defined(CONFIG_ITG3205_BW_98HZ)
#define ITG3205_BW 0x02
#elif defined(CONFIG_ITG3205_BW_188HZ)
#define ITG3205_BW 0x01
#elif defined(CONFIG_ITG3205_BW_256HZ)
#define ITG3205_BW 0x00
#else
#error "No filter bandwith assigned"
#endif

#define ITG3205_REG_TEMP_MSB 0x1B
#define ITG3205_REG_TEMP_LSB 0x1C
#define ITG3205_REG_GYRO_X_MSB 0x1D
#define ITG3205_REG_GYRO_X_LSB 0x1E
#define ITG3205_REG_GYRO_Y_MSB 0x1F
#define ITG3205_REG_GYRO_Y_LSB 0x20
#define ITG3205_REG_GYRO_Z_MSB 0x21
#define ITG3205_REG_GYRO_Z_LSB 0x22

#define ITG3205_REG_PWRMGMT 0x3E
#define ITG3205_CLK_SEL 0x02

struct itg3205_sample {
  uint16_t x;
  uint16_t y;
  uint16_t z;
};

struct itg3205_data {
  struct itg3205_sample sample;
  uint16_t temp;
};

struct itg3205_config {
  struct i2c_dt_spec i2c;
};

#endif /* __SENSOR_ITG3205__ */
