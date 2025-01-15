/*
 * Copyright (c) 2024 J.R. Versteegh
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * sensor_l3gd20h.c - Driver for L3GD20H gyro
 */

#define DT_DRV_COMPAT st_l3gd20h

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/byteorder.h>

#include "l3gd20h.h"

LOG_MODULE_REGISTER(L3GD20H, CONFIG_SENSOR_LOG_LEVEL);

#define GYRO_FS_245_LSB_PER_RPS 6548
#define GYRO_FS_500_LSB_PER_RPS 3274
#define GYRO_FS_2000_LSB_PER_RPS 819

#if defined(CONFIG_L3GD20H_SET_RUNTIME)
static inline int l3gd20h_set_odr_raw(const struct device* dev, uint8_t odr) {
  const struct l3gd20h_config* config = dev->config;

  return i2c_reg_update_byte_dt(
      &config->i2c,
      L3GD20H_REG_CTRL_REG1,
      L3GD20H_REG_CTRL_REG1_ODR_MASK,
      odr << L3GD20H_REG_CTRL_REG1_ODR_SHIFT);
}

static const struct {
  int freq_int;
  int freq_micro;
} l3gd20h_odr_map[] = {{12, 500000}, {25, 0}, {50, 0}, {100, 0}, {200, 0}, {400, 0}, {800, 0}};

static int l3gd20h_set_odr(const struct device* dev, const struct sensor_value* val) {
  uint8_t i;

  for (i = 0U; i < ARRAY_SIZE(l3gd20h_odr_map); ++i) {
    if (val->val1 < l3gd20h_odr_map[i].freq_int ||
        (val->val1 == l3gd20h_odr_map[i].freq_int &&
         val->val2 <= l3gd20h_odr_map[i].freq_micro)) {
      return l3gd20h_set_odr_raw(dev, i);
    }
  }

  return -ENOTSUP;
}

static inline int l3gd20h_set_fs_raw(const struct device* dev, uint8_t fs) {
  const struct l3gd20h_config* config = dev->config;
  struct l3gd20h_data* data = dev->data;

  if (i2c_reg_update_byte_dt(
          &config->i2c,
          L3GD20H_REG_CTRL_REG4,
          L3GD20H_REG_CTRL_REG4_FS_MASK,
          fs << L3GD20H_REG_CTRL_REG4_FS_SHIFT) < 0) {
    return -EIO;
  }

  data->fs = fs;

  return 0;
}

static const struct {
  int fs;
} l3gd20h_fs_map[] = {{245}, {500}, {2000}};

static int l3gd20h_set_fs(const struct device* dev, int val) {
  uint8_t i;

  for (i = 0U; i < ARRAY_SIZE(l3gd20h_fs_map); ++i) {
    if (val <= l3gd20h_fs_map[i].fs) {
      return l3gd20h_set_fs_raw(dev, i);
    }
  }

  return -ENOTSUP;
}
#endif

static inline int read_i2c(
    const struct device* dev, uint8_t const reg_addr, uint8_t* buf, uint32_t const num_bytes) {
  const struct l3gd20h_config* config = dev->config;
  if (i2c_burst_read_dt(&config->i2c, reg_addr | 128, buf, num_bytes) < 0) {
    LOG_DBG("Could not read L3GD20H data");
    return -EIO;
  }
  return 0;
}

static int read_i2c_byte(const struct device* dev, uint8_t const reg_addr, uint8_t* buf) {
  const struct l3gd20h_config* config = dev->config;
  if (i2c_reg_read_byte_dt(&config->i2c, reg_addr, buf) < 0) {
    LOG_DBG("Could not read L3GD20H data");
    return -EIO;
  }
  return 0;
}

static int l3gd20h_sample_fetch(const struct device* dev, enum sensor_channel chan) {
  struct l3gd20h_data* data = dev->data;
  switch (chan) {
    case SENSOR_CHAN_DIE_TEMP:
      return read_i2c_byte(dev, L3GD20H_REG_OUT_TEMP, (uint8_t*)&data->temp);
    case SENSOR_CHAN_GYRO_X:
      return read_i2c(
          dev, L3GD20H_REG_OUT_X_L, (uint8_t*)&data->sample.x, sizeof(data->sample.x));
    case SENSOR_CHAN_GYRO_Y:
      return read_i2c(
          dev, L3GD20H_REG_OUT_Y_L, (uint8_t*)&data->sample.y, sizeof(data->sample.y));
    case SENSOR_CHAN_GYRO_Z:
      return read_i2c(
          dev, L3GD20H_REG_OUT_Z_L, (uint8_t*)&data->sample.z, sizeof(data->sample.z));
    case SENSOR_CHAN_GYRO_XYZ:
      return read_i2c(dev, L3GD20H_REG_OUT_X_L, (uint8_t*)&data->sample, sizeof(data->sample));
    case SENSOR_CHAN_ALL:
      return read_i2c(dev, L3GD20H_REG_OUT_X_L, (uint8_t*)&data->sample, sizeof(data->sample));
    default:
      return -EINVAL;
  }

  return 0;
}

static inline struct sensor_value
get_sensor_value(uint16_t const value, int32_t const divisor) {
  int64_t val = (int16_t)sys_le16_to_cpu(value);
  int64_t tmp = val * 1000000 / divisor;
  struct sensor_value result = {.val1 = tmp / 1000000, .val2 = tmp % 1000000};
  return result;
}

static inline int l3gd20h_get_channel(
    enum sensor_channel chan,
    struct sensor_value* val,
    struct l3gd20h_data* data,
    int divisor) {
  switch (chan) {
    case SENSOR_CHAN_DIE_TEMP:
      val->val1 = -data->temp;
      val->val2 = 0;
      break;
    case SENSOR_CHAN_GYRO_X:
      *val = get_sensor_value(data->sample.x, divisor);
      break;
    case SENSOR_CHAN_GYRO_Y:
      *val = get_sensor_value(data->sample.y, divisor);
      break;
    case SENSOR_CHAN_GYRO_Z:
      *val = get_sensor_value(data->sample.z, divisor);
      break;
    case SENSOR_CHAN_GYRO_XYZ:
      *val = get_sensor_value(data->sample.x, divisor);
      *(val + 1) = get_sensor_value(data->sample.y, divisor);
      *(val + 2) = get_sensor_value(data->sample.z, divisor);
      break;
    default:
      return -ENOTSUP;
  }

  return 0;
}

static inline int
l3gd20h_get_gyro(const struct device* dev, enum sensor_channel chan, struct sensor_value* val) {
  struct l3gd20h_data* data = dev->data;

#if defined(CONFIG_L3GD20H_SET_RUNTIME)
  switch (data->fs) {
    case 0:
      return l3gd20h_get_channel(chan, val, data, GYRO_FS_245_LSB_PER_RPS);
    case 1:
      return l3gd20h_get_channel(chan, val, data, GYRO_FS_500_LSB_PER_RPS);
    case 2:
    case 3:
      return l3gd20h_get_channel(chan, val, data, GYRO_FS_2000_LSB_PER_RPS);
    default:
      return -ENOTSUP;
  }
#elif defined(CONFIG_L3GD20H_FS_245)
  return l3gd20h_get_channel(chan, val, data, GYRO_FS_245_LSB_PER_RPS);
#elif defined(CONFIG_L3GD20H_FS_500)
  return l3gd20h_get_channel(chan, val, data, GYRO_FS_500_LSB_PER_RPS);
#elif defined(CONFIG_L3GD20H_FS_2000)
  return l3gd20h_get_channel(chan, val, data, GYRO_FS_2000_LSB_PER_RPS);
#else
#error "Full scale not set"
#endif

  return 0;
}

static int l3gd20h_channel_get(
    const struct device* dev, enum sensor_channel chan, struct sensor_value* val) {
  switch (chan) {
    case SENSOR_CHAN_GYRO_X:
    case SENSOR_CHAN_GYRO_Y:
    case SENSOR_CHAN_GYRO_Z:
    case SENSOR_CHAN_GYRO_XYZ:
      return l3gd20h_get_gyro(dev, chan, val);
    default:
      return -ENOTSUP;
  }
}

#if defined(L3GD20H_SET_RUNTIME)
static inline int l3gd20h_attr_set_channels(
    const struct device* dev, enum sensor_attribute attr, const struct sensor_value* val) {
  switch (attr) {
    case SENSOR_ATTR_SAMPLING_FREQUENCY:
      return l3gd20h_set_odr(dev, val);
    case SENSOR_ATTR_FULL_SCALE:
      return l3gd20h_set_fs(dev, val);
    default:
      return -ENOTSUP;
  }

  return 0;
}

static int l3gd20h_attr_set(
    const struct device* dev,
    enum sensor_channel chan,
    enum sensor_attribute attr,
    const struct sensor_value* val) {

  switch (chan) {
    case SENSOR_CHAN_ALL:
    case SENSOR_CHAN_GYRO_X:
    case SENSOR_CHAN_GYRO_Y:
    case SENSOR_CHAN_GYRO_Z:
    case SENSOR_CHAN_GYRO_XYZ:
      return l3gd20h_attr_set_channels(dev, attr, val);
    default:
      return -ENOTSUP;
  }

  return 0;
}
#endif

static const struct sensor_driver_api l3gd20h_api_funcs = {
    .sample_fetch = l3gd20h_sample_fetch,
    .channel_get = l3gd20h_channel_get,
#if defined(L3GD20H_SET_RUNTIME)
    .attr_set = l3gd20h_attr_set,
#endif
};

static int l3gd20h_init_chip(const struct device* dev) {
  const struct l3gd20h_config* config = dev->config;
  uint8_t chip_id;

  if (i2c_reg_read_byte_dt(&config->i2c, L3GD20H_REG_WHO_AM_I, &chip_id) < 0) {
    LOG_DBG("Failed reading chip id");
    return -EIO;
  }

  if (chip_id != L3GD20H_VAL_WHO_AM_I) {
    LOG_DBG("Invalid chip id 0x%x", chip_id);
    return -EIO;
  }

  LOG_DBG("chip id 0x%x", chip_id);

  uint8_t data = L3GD20H_DEFAULT_ODR | L3GD20H_DEFAULT_BW | L3GD20H_REG_CTRL_REG1_POWER_UP;
  if (i2c_reg_write_byte_dt(&config->i2c, L3GD20H_REG_CTRL_REG1, data) < 0) {
    LOG_DBG("Failed to set ODR and BW");
    return -EIO;
  }
  LOG_DBG("Written %d to CTRL1", (int)data);

  data = L3GD20H_DEFAULT_FS;
  if (i2c_reg_write_byte_dt(&config->i2c, L3GD20H_REG_CTRL_REG4, data) < 0) {
    LOG_DBG("Failed to set FS");
    return -EIO;
  }
  LOG_DBG("Written %d to CTRL4", (int)data);

  data = L3GD20H_DEFAULT_LOW_ODR;
  if (i2c_reg_write_byte_dt(&config->i2c, L3GD20H_REG_LOW_ODR, data) < 0) {
    LOG_DBG("Failed to set LOW_ODR");
    return -EIO;
  }
  LOG_DBG("Written %d to LOW_ODR", (int)data);

  return 0;
}

int l3gd20h_init(const struct device* dev) {
  const struct l3gd20h_config* const config = dev->config;

  if (!device_is_ready(config->i2c.bus)) {
    LOG_ERR("Bus device is not ready");
    return -ENODEV;
  }

  if (l3gd20h_init_chip(dev) < 0) {
    LOG_DBG("failed to initialize chip");
    return -EIO;
  }

  return 0;
}

#define L3GD20H_DEFINE(inst)                                                                   \
  static struct l3gd20h_data l3gd20h_data_##inst;                                              \
                                                                                               \
  static const struct l3gd20h_config l3gd20h_config_##inst = {                                 \
      .i2c = I2C_DT_SPEC_INST_GET(inst),                                                       \
  };                                                                                           \
                                                                                               \
  SENSOR_DEVICE_DT_INST_DEFINE(                                                                \
      inst,                                                                                    \
      l3gd20h_init,                                                                            \
      NULL,                                                                                    \
      &l3gd20h_data_##inst,                                                                    \
      &l3gd20h_config_##inst,                                                                  \
      POST_KERNEL,                                                                             \
      CONFIG_SENSOR_INIT_PRIORITY,                                                             \
      &l3gd20h_api_funcs);

DT_INST_FOREACH_STATUS_OKAY(L3GD20H_DEFINE)
