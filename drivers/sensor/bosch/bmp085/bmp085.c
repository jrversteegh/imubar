/*
 * Copyright (c) 2024 J.R. Versteegh
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT bosch_bmp085

#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>

#include "bmp085.h"

LOG_MODULE_REGISTER(BMP085, CONFIG_SENSOR_LOG_LEVEL);

static int bmp085_sample_fetch(const struct device *dev,
                               const enum sensor_channel chan) {
  struct bmp085_data *data = dev->data;
}

static int bmp085_channel_get(const struct device *dev,
                              enum sensor_channel chan,
                              struct sensor_value *val) {
  struct bmp085_data *data = dev->data;
}

int bmp085_init(const struct device *dev) {
  const struct bmp085_config *config = dev->config;
  uint8_t id = 0U;
  int result = 0;

  if (!device_is_ready(config->i2c.bus)) {
    LOG_ERR("I2C bus device not ready");
    return -ENODEV;
  }

  /* read device ID
  if (i2c_reg_read_byte_dt(&config->i2c, bmp085_CHIP_ID_ADDR, &id) < 0) {
    LOG_DBG("Could not read chip id");
    return -EIO;
  }

  if (id != bmp085_CHIP_ID) {
    LOG_DBG("Unexpected chip id (%x)", id);
    return -EIO;
  }
  */
  return 0;
}

static int bmp085_attr_set(const struct device *dev, enum sensor_channel chan,
                           enum sensor_attribute attr,
                           const struct sensor_value *val) {
  return -ENOTSUP;
}

static int bmp085_attr_get(const struct device *dev, enum sensor_channel chan,
                           enum sensor_attribute attr,
                           struct sensor_value *val) {
  return -ENOTSUP;
}

static const struct sensor_driver_api bmp085_driver_api = {
    .attr_set = bmp085_attr_set,
    .attr_get = bmp085_attr_get,
    .sample_fetch = bmp085_sample_fetch,
    .channel_get = bmp085_channel_get,
};

#define BMP085_DEFINE(inst)                                                    \
  static struct bmp085_data bmp085_data_##inst;                                \
                                                                               \
  static const struct bmp085_config bmp085_config##inst = {                    \
      .i2c = I2C_DT_SPEC_INST_GET(inst)};                                      \
                                                                               \
  SENSOR_DEVICE_DT_INST_DEFINE(                                                \
      inst, bmp085_init, NULL, &bmp085_data_##inst, &bmp085_config##inst,      \
      POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY, &bmp085_driver_api);

DT_INST_FOREACH_STATUS_OKAY(BMP085_DEFINE)
