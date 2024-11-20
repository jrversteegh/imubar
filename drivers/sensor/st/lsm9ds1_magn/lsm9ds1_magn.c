/*
 * Copyright (c) 2024 J.R. Versteegh
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * sensor_lsm9ds1_magn.c - Driver for LSM9DS1 magnetometer
 */

#define DT_DRV_COMPAT st_lsm9ds1_magn

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "lsm9ds1_magn.h"

LOG_MODULE_REGISTER(LSM9DS1_MAGN, CONFIG_SENSOR_LOG_LEVEL);

#define MAGN_FS_4_LSB_PER_MG 7143
#define MAGN_FS_8_LSB_PER_MG 3448
#define MAGN_FS_12_LSB_PER_MG 2326
#define MAGN_FS_16_LSB_PER_MG 1724

#if defined(CONFIG_LSM9DS1_MAGN_SET_RUNTIME)
static inline int lsm9ds1_magn_set_odr_raw(const struct device *dev,
                                           uint8_t odr) {
  const struct lsm9ds1_magn_config *config = dev->config;

  return i2c_reg_update_byte_dt(&config->i2c, LSM9DS1_MAGN_REG_CTRL_REG1,
                                LSM9DS1_MAGN_REG_CTRL_REG1_ODR_MASK,
                                odr << LSM9DS1_MAGN_REG_CTRL_REG1_ODR_SHIFT);
}

static const struct {
  int freq_int;
  int freq_micro;
} lsm9ds1_magn_odr_map[] = {{0, 625000}, {1, 250000}, {2, 500000}, {5, 0},
                            {10, 0},     {20, 0},     {40, 0},     {80, 0}};

static int lsm9ds1_magn_set_odr(const struct device *dev,
                                const struct sensor_value *val) {
  uint8_t i;

  for (i = 0U; i < ARRAY_SIZE(lsm9ds1_magn_odr_map); ++i) {
    if (val->val1 < lsm9ds1_magn_odr_map[i].freq_int ||
        (val->val1 == lsm9ds1_magn_odr_map[i].freq_int &&
         val->val2 <= lsm9ds1_magn_odr_map[i].freq_micro)) {
      return lsm9ds1_magn_set_odr_raw(dev, i);
    }
  }

  return -ENOTSUP;
}

static inline int lsm9ds1_magn_set_fs_raw(const struct device *dev,
                                          uint8_t fs) {
  const struct lsm9ds1_magn_config *config = dev->config;
  struct lsm9ds1_magn_data *data = dev->data;

  if (i2c_reg_update_byte_dt(&config->i2c, LSM9DS1_MAGN_REG_CTRL_REG2,
                             LSM9DS1_MAGN_REG_CTRL_REG2_FS_MASK,
                             fs << LSM9DS1_MAGN_REG_CTRL_REG2_FS_SHIFT) < 0) {
    return -EIO;
  }

  data->fs = fs;

  return 0;
}

static const struct {
  int fs;
} lsm9ds1_magn_fs_map[] = {{4}, {8}, {12}, {16}};

static int lsm9ds1_magn_set_fs(const struct device *dev, int val) {
  uint8_t i;

  for (i = 0U; i < ARRAY_SIZE(lsm9ds1_magn_fs_map); ++i) {
    if (val <= lsm9ds1_magn_fs_map[i].fs) {
      return lsm9ds1_magn_set_fs_raw(dev, i);
    }
  }

  return -ENOTSUP;
}
#endif

static inline int lsm9ds1_magn_sample_fetch_magn_x(const struct device *dev) {
  struct lsm9ds1_magn_data *data = dev->data;
  const struct lsm9ds1_magn_config *config = dev->config;
  uint8_t out_l, out_h;

  if (i2c_reg_read_byte_dt(&config->i2c, LSM9DS1_MAGN_REG_OUT_X_L, &out_l) <
          0 ||
      i2c_reg_read_byte_dt(&config->i2c, LSM9DS1_MAGN_REG_OUT_X_H, &out_h) <
          0) {
    LOG_DBG("failed to read magn sample (X axis)");
    return -EIO;
  }

  data->sample_magn_x = (int16_t)((uint16_t)(out_l) | ((uint16_t)(out_h) << 8));

  return 0;
}

static inline int lsm9ds1_magn_sample_fetch_magn_y(const struct device *dev) {
  struct lsm9ds1_magn_data *data = dev->data;
  const struct lsm9ds1_magn_config *config = dev->config;
  uint8_t out_l, out_h;

  if (i2c_reg_read_byte_dt(&config->i2c, LSM9DS1_MAGN_REG_OUT_Y_L, &out_l) <
          0 ||
      i2c_reg_read_byte_dt(&config->i2c, LSM9DS1_MAGN_REG_OUT_Y_H, &out_h) <
          0) {
    LOG_DBG("failed to read magn sample (Y axis)");
    return -EIO;
  }

  data->sample_magn_y = (int16_t)((uint16_t)(out_l) | ((uint16_t)(out_h) << 8));
  return 0;
}

static inline int lsm9ds1_magn_sample_fetch_magn_z(const struct device *dev) {
  struct lsm9ds1_magn_data *data = dev->data;
  const struct lsm9ds1_magn_config *config = dev->config;
  uint8_t out_l, out_h;
  if (i2c_reg_read_byte_dt(&config->i2c, LSM9DS1_MAGN_REG_OUT_Z_L, &out_l) <
          0 ||
      i2c_reg_read_byte_dt(&config->i2c, LSM9DS1_MAGN_REG_OUT_Z_H, &out_h) <
          0) {
    LOG_DBG("failed to read magn sample (Z axis)");
    return -EIO;
  }

  data->sample_magn_z = (int16_t)((uint16_t)(out_l) | ((uint16_t)(out_h) << 8));
  return 0;
}

static inline int lsm9ds1_magn_sample_fetch_magn(const struct device *dev) {
  struct lsm9ds1_magn_data *data = dev->data;
  const struct lsm9ds1_magn_config *config = dev->config;

  uint8_t buf[6];
  if (i2c_burst_read_dt(&config->i2c, LSM9DS1_MAGN_REG_OUT_X_L, buf,
                        sizeof(buf)) < 0) {
    LOG_DBG("Could not read LSM9DS1 data");
    return -EIO;
  }
  data->sample_magn_x =
      (int16_t)((uint16_t)(buf[0]) | ((uint16_t)(buf[1]) << 8));
  data->sample_magn_y =
      (int16_t)((uint16_t)(buf[2]) | ((uint16_t)(buf[3]) << 8));
  data->sample_magn_z =
      (int16_t)((uint16_t)(buf[4]) | ((uint16_t)(buf[5]) << 8));
  return 0;
}

static int lsm9ds1_magn_sample_fetch(const struct device *dev,
                                     enum sensor_channel chan) {
  switch (chan) {
  case SENSOR_CHAN_MAGN_X:
    return lsm9ds1_magn_sample_fetch_magn_x(dev);
  case SENSOR_CHAN_MAGN_Y:
    return lsm9ds1_magn_sample_fetch_magn_y(dev);
  case SENSOR_CHAN_MAGN_Z:
    return lsm9ds1_magn_sample_fetch_magn_z(dev);
  case SENSOR_CHAN_MAGN_XYZ:
    return lsm9ds1_magn_sample_fetch_magn(dev);
  case SENSOR_CHAN_ALL:
    return lsm9ds1_magn_sample_fetch_magn(dev);
  default:
    return -EINVAL;
  }

  return 0;
}

static inline struct sensor_value get_sensor_value(const uint16_t value,
                                                   const int32_t divisor) {
  int32_t tmp = (int64_t)value * 1000000 / divisor;
  struct sensor_value result = {.val1 = tmp / 1000000, .val2 = tmp % 1000000};
  return result;
}

static inline int lsm9ds1_magn_get_channel(enum sensor_channel chan,
                                           struct sensor_value *val,
                                           struct lsm9ds1_magn_data *data,
                                           int divisor) {
  switch (chan) {
  case SENSOR_CHAN_MAGN_X:
    *val = get_sensor_value(data->sample_magn_x, divisor);
    break;
  case SENSOR_CHAN_MAGN_Y:
    *val = get_sensor_value(data->sample_magn_y, divisor);
    break;
  case SENSOR_CHAN_MAGN_Z:
    *val = get_sensor_value(data->sample_magn_z, divisor);
    break;
  case SENSOR_CHAN_MAGN_XYZ:
    *val = get_sensor_value(data->sample_magn_x, divisor);
    *(val + 1) = get_sensor_value(data->sample_magn_y, divisor);
    *(val + 2) = get_sensor_value(data->sample_magn_z, divisor);
    break;
  default:
    return -ENOTSUP;
  }

  return 0;
}

static inline int lsm9ds1_magn_get_magn(const struct device *dev,
                                        enum sensor_channel chan,
                                        struct sensor_value *val) {
  struct lsm9ds1_magn_data *data = dev->data;

#if defined(CONFIG_LSM9DS1_MAGN_SET_RUNTIME)
  switch (data->fs) {
  case 0:
    return lsm9ds1_magn_get_channel(chan, val, data, MAGN_FS_4_LSB_PER_MG);
  case 1:
    return lsm9ds1_magn_get_channel(chan, val, data, MAGN_FS_8_LSB_PER_MG);
  case 2:
    return lsm9ds1_magn_get_channel(chan, val, data, MAGN_FS_12_LSB_PER_MG);
  case 3:
    return lsm9ds1_magn_get_channel(chan, val, data, MAGN_FS_16_LSB_PER_MG);
  default:
    return -ENOTSUP;
  }
#elif defined(CONFIG_LSM9DS1_MAGN_FULL_SCALE_4)
  return lsm9ds1_magn_get_channel(chan, val, data, MAGN_FS_4_LSB_PER_MG);
#elif defined(CONFIG_LSM9DS1_MAGN_FULL_SCALE_8)
  return lsm9ds1_magn_get_channel(chan, val, data, MAGN_FS_8_LSB_PER_MG);
#elif defined(CONFIG_LSM9DS1_MAGN_FULL_SCALE_12)
  return lsm9ds1_magn_get_channel(chan, val, data, MAGN_FS_12_LSB_PER_MG);
#elif defined(CONFIG_LSM9DS1_MAGN_FULL_SCALE_16)
  return lsm9ds1_magn_get_channel(chan, val, data, MAGN_FS_16_LSB_PER_MG);
#else
#error "Full scale not set"
#endif

  return 0;
}

static int lsm9ds1_magn_channel_get(const struct device *dev,
                                    enum sensor_channel chan,
                                    struct sensor_value *val) {
  switch (chan) {
  case SENSOR_CHAN_MAGN_X:
  case SENSOR_CHAN_MAGN_Y:
  case SENSOR_CHAN_MAGN_Z:
  case SENSOR_CHAN_MAGN_XYZ:
    return lsm9ds1_magn_get_magn(dev, chan, val);
  default:
    return -ENOTSUP;
  }
}

#if defined(LSM9DS1_MAGN_SET_RUNTIME)
static inline int
lsm9ds1_magn_attr_set_channels(const struct device *dev,
                               enum sensor_attribute attr,
                               const struct sensor_value *val) {
  switch (attr) {
  case SENSOR_ATTR_SAMPLING_FREQUENCY:
    return lsm9ds1_magn_set_odr(dev, val);
  case SENSOR_ATTR_FULL_SCALE:
    return lsm9ds1_magn_set_fs(dev, val);
  default:
    return -ENOTSUP;
  }

  return 0;
}

static int lsm9ds1_magn_attr_set(const struct device *dev,
                                 enum sensor_channel chan,
                                 enum sensor_attribute attr,
                                 const struct sensor_value *val) {

  switch (chan) {
  case SENSOR_CHAN_MAGN_X:
  case SENSOR_CHAN_MAGN_Y:
  case SENSOR_CHAN_MAGN_Z:
  case SENSOR_CHAN_MAGN_XYZ:
    return lsm9ds1_magn_attr_set_channels(dev, attr, val);
  default:
    return -ENOTSUP;
  }

  return 0;
}
#endif

static const struct sensor_driver_api lsm9ds1_magn_api_funcs = {
    .sample_fetch = lsm9ds1_magn_sample_fetch,
    .channel_get = lsm9ds1_magn_channel_get,
#if defined(LSM9DS1_MAGN_SET_RUNTIME)
    .attr_set = lsm9ds1_magn_attr_set,
#endif
};

static int lsm9ds1_magn_init_chip(const struct device *dev) {
  const struct lsm9ds1_magn_config *config = dev->config;
  uint8_t chip_id;

  if (i2c_reg_read_byte_dt(&config->i2c, LSM9DS1_MAGN_REG_WHO_AM_I, &chip_id) <
      0) {
    LOG_DBG("failed reading chip id");
    return -EIO;
  }

  if (chip_id != LSM9DS1_MAGN_VAL_WHO_AM_I) {
    LOG_DBG("invalid chip id 0x%x", chip_id);
    return -EIO;
  }

  LOG_DBG("chip id 0x%x", chip_id);

  if (i2c_reg_write_byte_dt(&config->i2c, LSM9DS1_MAGN_REG_CTRL_REG1,
                            LSM9DS1_MAGN_DEFAULT_TEMP_COMP |
                                LSM9DS1_MAGN_DEFAULT_ODR |
                                LSM9DS1_MAGN_DEFAULT_XY_OPR)) {
    LOG_DBG("failed to set X OPR");
    return -EIO;
  }

  if (i2c_reg_write_byte_dt(&config->i2c, LSM9DS1_MAGN_REG_CTRL_REG2,
                            LSM9DS1_MAGN_DEFAULT_FS)) {
    LOG_DBG("failed to set FS");
    return -EIO;
  }

  if (i2c_reg_write_byte_dt(&config->i2c, LSM9DS1_MAGN_REG_CTRL_REG4,
                            LSM9DS1_MAGN_DEFAULT_Z_OPR)) {
    LOG_DBG("failed to set Z OPR");
    return -EIO;
  }

  return 0;
}

int lsm9ds1_magn_init(const struct device *dev) {
  const struct lsm9ds1_magn_config *const config = dev->config;

  if (!device_is_ready(config->i2c.bus)) {
    LOG_ERR("Bus device is not ready");
    return -ENODEV;
  }

  if (lsm9ds1_magn_init_chip(dev) < 0) {
    LOG_DBG("failed to initialize chip");
    return -EIO;
  }

  return 0;
}

#define LSM9DS1_MAGN_DEFINE(inst)                                              \
  static struct lsm9ds1_magn_data lsm9ds1_magn_data_##inst;                    \
                                                                               \
  static const struct lsm9ds1_magn_config lsm9ds1_magn_config_##inst = {       \
      .i2c = I2C_DT_SPEC_INST_GET(inst),                                       \
  };                                                                           \
                                                                               \
  SENSOR_DEVICE_DT_INST_DEFINE(                                                \
      inst, lsm9ds1_magn_init, NULL, &lsm9ds1_magn_data_##inst,                \
      &lsm9ds1_magn_config_##inst, POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY,   \
      &lsm9ds1_magn_api_funcs);

DT_INST_FOREACH_STATUS_OKAY(LSM9DS1_MAGN_DEFINE)
