/*
 * Copyright (c) 2024 J.R. Versteegh
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT invensense_itg3205

#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/byteorder.h>

#include "itg3205.h"

#define ITG3205_LSB_PER_RAD 824
#define ITG3205_LSB_PER_DEG 280
#define ITG3205_TEMP_OFFSET -47

LOG_MODULE_REGISTER(ITG3205, CONFIG_SENSOR_LOG_LEVEL);

static inline struct sensor_value
get_sensor_value(uint16_t const value, int32_t const divisor) {
  int64_t val = (int16_t)sys_be16_to_cpu(value);
  int64_t tmp = val * 1000000 / divisor;
  struct sensor_value result = {.val1 = tmp / 1000000, .val2 = tmp % 1000000};
  return result;
}

static int itg3205_channel_get(
    const struct device* dev, enum sensor_channel chan, struct sensor_value* val) {
  struct itg3205_data* data = dev->data;

  switch (chan) {
    case SENSOR_CHAN_GYRO_X:
      *val = get_sensor_value(data->sample.x, ITG3205_LSB_PER_RAD);
      break;
    case SENSOR_CHAN_GYRO_Y:
      *val = get_sensor_value(data->sample.y, ITG3205_LSB_PER_RAD);
      break;
    case SENSOR_CHAN_GYRO_Z:
      *val = get_sensor_value(data->sample.z, ITG3205_LSB_PER_RAD);
      break;
    case SENSOR_CHAN_GYRO_XYZ:
      *val = get_sensor_value(data->sample.x, ITG3205_LSB_PER_RAD);
      *(val + 1) = get_sensor_value(data->sample.y, ITG3205_LSB_PER_RAD);
      *(val + 2) = get_sensor_value(data->sample.z, ITG3205_LSB_PER_RAD);
      break;
    case SENSOR_CHAN_DIE_TEMP:
      *val = get_sensor_value(data->temp, ITG3205_LSB_PER_DEG);
      val->val1 += ITG3205_TEMP_OFFSET;
      break;
    default:
      return -ENOTSUP;
  }

  return 0;
}

static int itg3205_sample_fetch(const struct device* dev, enum sensor_channel chan) {
  struct itg3205_data* data = dev->data;
  const struct itg3205_config* config = dev->config;

  switch (chan) {
    case SENSOR_CHAN_ALL:
    case SENSOR_CHAN_GYRO_XYZ:
      if (i2c_burst_read_dt(
              &config->i2c,
              ITG3205_REG_GYRO_X_MSB,
              (uint8_t*)&data->sample,
              sizeof(data->sample)) < 0) {
        LOG_ERR("Failed to read gyro sample.");
        return -EIO;
      }
      break;
    case SENSOR_CHAN_DIE_TEMP:
      if (i2c_burst_read_dt(&config->i2c, ITG3205_REG_TEMP_MSB, (uint8_t*)&data->temp, 2) < 0) {
        LOG_ERR("Failed to read temp sample.");
        return -EIO;
      }
      break;
    default:
      return -ENOTSUP;
  }

  return 0;
}

static const struct sensor_driver_api itg3205_driver_api = {
    .sample_fetch = itg3205_sample_fetch,
    .channel_get = itg3205_channel_get,
};

int itg3205_init(const struct device* dev) {
  const struct itg3205_config* config = dev->config;
  uint8_t id;

  if (!device_is_ready(config->i2c.bus)) {
    LOG_ERR("Bus device is not ready");
    return -ENODEV;
  }

  if (i2c_reg_read_byte_dt(&config->i2c, ITG3205_REG_CHIP_ID, &id) < 0) {
    LOG_ERR("Failed to read chip ID.");
    return -EIO;
  }

  if (id != ITG3205_CHIP_ID) {
    LOG_DBG("Unexpected chip id (%x)", id);
    return -EIO;
  }

  if (i2c_reg_write_byte_dt(
          &config->i2c,
          ITG3205_REG_BW,
          (ITG3205_BW << ITG3205_BW_SHIFT) | (ITG3205_FS_SEL << ITG3205_FS_SEL_SHIFT)) < 0) {
    LOG_ERR("Failed to set filter bandwith and full range selection.");
    return -EIO;
  }

  if (i2c_reg_write_byte_dt(
          &config->i2c, ITG3205_REG_SR_DIVIDER, CONFIG_ITG3205_SAMPLE_RATE_DIVIDER) < 0) {
    LOG_ERR("Failed to write sample rate divider");
    return -EIO;
  }

  // Select gyro clock instead of internal oscillator as per datasheet
  // recommendation
  if (i2c_reg_write_byte_dt(&config->i2c, ITG3205_REG_PWRMGMT, ITG3205_CLK_SEL) < 0) {
    LOG_ERR("Failed to set CLK_SEL.");
    return -EIO;
  }

  return 0;
}

#define ITG3205_DEFINE(inst)                                                                   \
  static struct itg3205_data itg3205_data_##inst;                                              \
                                                                                               \
  static const struct itg3205_config itg3205_config_##inst = {                                 \
      .i2c = I2C_DT_SPEC_INST_GET(inst),                                                       \
  };                                                                                           \
                                                                                               \
  SENSOR_DEVICE_DT_INST_DEFINE(                                                                \
      inst,                                                                                    \
      itg3205_init,                                                                            \
      NULL,                                                                                    \
      &itg3205_data_##inst,                                                                    \
      &itg3205_config_##inst,                                                                  \
      POST_KERNEL,                                                                             \
      CONFIG_SENSOR_INIT_PRIORITY,                                                             \
      &itg3205_driver_api);

DT_INST_FOREACH_STATUS_OKAY(ITG3205_DEFINE)
