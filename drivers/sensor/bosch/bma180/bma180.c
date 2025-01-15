/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT bosch_bma180

#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/byteorder.h>

#include "bma180.h"

LOG_MODULE_REGISTER(BMA180, CONFIG_SENSOR_LOG_LEVEL);

static int bma180_sample_fetch(const struct device* dev, enum sensor_channel chan) {
  struct bma180_data* data = dev->data;
  const struct bma180_config* config = dev->config;
  switch (chan) {
    case SENSOR_CHAN_ALL:
    case SENSOR_CHAN_ACCEL_XYZ:
      if (i2c_burst_read_dt(
              &config->i2c,
              BMA180_REG_ACCEL_X_LSB,
              (uint8_t*)&data->sample,
              sizeof(data->sample)) < 0) {
        LOG_DBG("Could not read accel axis data");
        return -EIO;
      }
      break;
    case SENSOR_CHAN_DIE_TEMP:
      if (i2c_reg_read_byte_dt(&config->i2c, BMA180_REG_TEMP, (uint8_t*)&data->temp) < 0) {
        LOG_DBG("Could not read temperature data");
        return -EIO;
      }
      break;
    default:
      return -ENOTSUP;
  }

  return 0;
}

static inline struct sensor_value get_sensor_value(uint16_t const value) {
  int64_t val = (int16_t)sys_le16_to_cpu(value);
  int64_t tmp = 1000000 * val / BMA180_LSB_PER_MS2;
  struct sensor_value result = {.val1 = tmp / 1000000, .val2 = tmp % 1000000};
  return result;
}

static int bma180_channel_get(
    const struct device* dev, enum sensor_channel chan, struct sensor_value* val) {
  struct bma180_data* data = dev->data;

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
    case SENSOR_CHAN_DIE_TEMP:
      val->val1 = data->temp;
      val->val2 = 0;
      break;
    default:
      return -ENOTSUP;
  }

  return 0;
}

static const struct sensor_driver_api bma180_driver_api = {
    .sample_fetch = bma180_sample_fetch,
    .channel_get = bma180_channel_get,
};

int bma180_init(const struct device* dev) {
  const struct bma180_config* config = dev->config;
  uint8_t id = 0U;

  if (!device_is_ready(config->i2c.bus)) {
    LOG_ERR("I2C bus device not ready");
    return -ENODEV;
  }

  if (i2c_reg_read_byte_dt(&config->i2c, BMA180_REG_CHIP_ID, &id) < 0) {
    LOG_DBG("Could not read chip id");
    return -EIO;
  }

  if (id != BMA180_CHIP_ID) {
    LOG_DBG("Unexpected chip id (%x)", id);
    return -EIO;
  }

  if (i2c_reg_write_byte_dt(
          &config->i2c,
          BMA180_REG_BW,
          (BMA180_BW << BMA180_BW_SHIFT) | (BMA180_TCS << BMA180_TCS_SHIFT)) < 0) {
    LOG_DBG("Could not set data filter bandwidth");
    return -EIO;
  }

  if (i2c_reg_write_byte_dt(
          &config->i2c, BMA180_REG_RANGE, BMA180_RANGE << BMA180_RANGE_SHIFT) < 0) {
    LOG_DBG("Could not set data g-range");
    return -EIO;
  }

  return 0;
}

#define BMA180_DEFINE(inst)                                                                    \
  static struct bma180_data bma180_data_##inst;                                                \
                                                                                               \
  static const struct bma180_config bma180_config##inst = {.i2c = I2C_DT_SPEC_INST_GET(inst)}; \
                                                                                               \
  SENSOR_DEVICE_DT_INST_DEFINE(                                                                \
      inst,                                                                                    \
      bma180_init,                                                                             \
      NULL,                                                                                    \
      &bma180_data_##inst,                                                                     \
      &bma180_config##inst,                                                                    \
      POST_KERNEL,                                                                             \
      CONFIG_SENSOR_INIT_PRIORITY,                                                             \
      &bma180_driver_api);

DT_INST_FOREACH_STATUS_OKAY(BMA180_DEFINE)
