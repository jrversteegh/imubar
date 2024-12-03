/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT st_h3lis331dl

#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/byteorder.h>

#include "h3lis331dl.h"

LOG_MODULE_REGISTER(H3LIS331DL, CONFIG_SENSOR_LOG_LEVEL);
char dumpstr[256];

static int h3lis331dl_sample_fetch(const struct device *dev,
                                   enum sensor_channel chan) {
  struct h3lis331dl_data *data = dev->data;
  const struct h3lis331dl_config *config = dev->config;
  switch (chan) {
  case SENSOR_CHAN_ALL:
  case SENSOR_CHAN_ACCEL_XYZ:
    if (i2c_burst_read_dt(&config->i2c, H3LIS331DL_REG_ACCEL_X_LSB | 0x80,
                          (uint8_t *)&data->sample, sizeof(data->sample)) < 0) {
      LOG_DBG("Could not read accel axis data");
      return -EIO;
    }
    break;
  default:
    return -ENOTSUP;
  }

  return 0;
}

static inline struct sensor_value get_sensor_value(const uint16_t value) {
  int64_t val = (int16_t)sys_le16_to_cpu(value);
  int64_t tmp = 1000000000LL * val / H3LIS331DL_LSB_PER_KMS2;
  struct sensor_value result = {.val1 = tmp / 1000000, .val2 = tmp % 1000000};
  return result;
}

static int h3lis331dl_channel_get(const struct device *dev,
                                  enum sensor_channel chan,
                                  struct sensor_value *val) {
  struct h3lis331dl_data *data = dev->data;

  switch (chan) {
  case SENSOR_CHAN_ACCEL_X:
    *val = get_sensor_value(data->sample.x);
    break;
  case SENSOR_CHAN_ACCEL_Y:
    *val = get_sensor_value(data->sample.y);
    break;
  case SENSOR_CHAN_ACCEL_Z:
    *val = get_sensor_value(data->sample.z);
    break;
  case SENSOR_CHAN_ACCEL_XYZ:
    *val = get_sensor_value(data->sample.x);
    *(val + 1) = get_sensor_value(data->sample.y);
    *(val + 2) = get_sensor_value(data->sample.z);
    break;
  default:
    return -ENOTSUP;
  }

  return 0;
}

static const struct sensor_driver_api h3lis331dl_driver_api = {
    .sample_fetch = h3lis331dl_sample_fetch,
    .channel_get = h3lis331dl_channel_get,
};

int h3lis331dl_init(const struct device *dev) {
  const struct h3lis331dl_config *config = dev->config;
  uint8_t id = 0U;

  if (!device_is_ready(config->i2c.bus)) {
    LOG_ERR("I2C bus device not ready");
    return -ENODEV;
  }

  if (i2c_reg_read_byte_dt(&config->i2c, H3LIS331DL_REG_CHIP_ID, &id) < 0) {
    LOG_DBG("Could not read chip id");
    return -EIO;
  }

  if (id != H3LIS331DL_CHIP_ID) {
    LOG_DBG("Unexpected chip id (%x)", id);
    return -EIO;
  }

  uint8_t ctrl_reg1 = (H3LIS331DL_AXES_ENABLE << H3LIS331DL_AXES_SHIFT) |
                      (H3LIS331DL_ODR << H3LIS331DL_ODR_SHIFT) |
                      (H3LIS331DL_PM_NORMAL << H3LIS331DL_PM_SHIFT);

  LOG_DBG("CTRL_REG1: %d", (int)ctrl_reg1);
  if (i2c_reg_write_byte_dt(&config->i2c, H3LIS331DL_REG_CTRL1, ctrl_reg1) <
      0) {
    LOG_DBG("Could not set data output rate");
    return -EIO;
  }

  uint8_t ctrl_reg4 = H3LIS331DL_RANGE << H3LIS331DL_RANGE_SHIFT;
  LOG_DBG("CTRL_REG4: %d", (int)ctrl_reg4);
  if (i2c_reg_write_byte_dt(&config->i2c, H3LIS331DL_REG_CTRL4, ctrl_reg4) <
      0) {
    LOG_DBG("Could not set data g-range");
    return -EIO;
  }

  return 0;
}

#define H3LIS331DL_DEFINE(inst)                                                \
  static struct h3lis331dl_data h3lis331dl_data_##inst;                        \
                                                                               \
  static const struct h3lis331dl_config h3lis331dl_config##inst = {            \
      .i2c = I2C_DT_SPEC_INST_GET(inst)};                                      \
                                                                               \
  SENSOR_DEVICE_DT_INST_DEFINE(                                                \
      inst, h3lis331dl_init, NULL, &h3lis331dl_data_##inst,                    \
      &h3lis331dl_config##inst, POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY,      \
      &h3lis331dl_driver_api);

DT_INST_FOREACH_STATUS_OKAY(H3LIS331DL_DEFINE)
