/* lsm9ds1.c - Driver for LSM9DS1 accelerometer and gyroscope sensor */

/*
 * Copyright (c) 2024 J.R. Versteegh
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT st_lsm9ds1_ag

#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/__assert.h>
#include <zephyr/sys/byteorder.h>

#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

#include "lsm9ds1_ag.h"

LOG_MODULE_REGISTER(LSM9DS1_AG, CONFIG_SENSOR_LOG_LEVEL);

#define DEG2RAD 0.01745329252f

#define TEMP_LSB_PER_C 16

#define ACCEL_FS_2_LSB_PER_MS 1671
#define ACCEL_FS_4_LSB_PER_MS 836
#define ACCEL_FS_8_LSB_PER_MS 418
#define ACCEL_FS_16_LSB_PER_MS 139

#define GYRO_FS_245_LSB_PER_RAD 6548
#define GYRO_FS_500_LSB_PER_RAD 3274
#define GYRO_FS_2000_LSB_PER_RAD 819

#if defined(CONFIG_LSM9DS1_SET_RUNTIME)
static int lsm9ds1_set_gyro_fs_raw(const struct device *dev, uint8_t fs) {
  const struct lsm9ds1_config *config = dev->config;

  if (i2c_reg_update_byte_dt(&config->i2c, LSM9DS1_REG_CTRL_REG1,
                             LSM9DS1_REG_CTRL_REG1_FS_G_MASK,
                             fs << LSM9DS1_REG_CTRL_REG1_FS_G_SHIFT) < 0) {
    return -EIO;
  }

  data->gyro_fs = fs;

  return 0;
}

static const struct {
  int fs;
  uint8_t reg_val;
} lsm9ds1_fs_table[] = {{245, 0}, {500, 1}, {2000, 3}};

static int lsm9ds1_set_gyro_fs(const struct device *dev, int fs) {
  int i;

  for (i = 0; i < ARRAY_SIZE(lsm9ds1_fs_table); ++i) {
    if (fs == lsm9ds1_fs_table[i].fs) {
      return lsm9ds1_set_fs_raw(dev, lsm9ds1_fs_table[i].reg_val);
    }
  }

  return -ENOTSUP;
}

static inline int lsm9ds1_set_gyro_odr_raw(const struct device *dev,
                                           uint8_t odr) {
  const struct lsm9ds1_config *config = dev->config;

  return i2c_reg_update_byte_dt(&config->i2c, LSM9DS1_REG_CTRL_REG1,
                                LSM9DS1_REG_CTRL_REG1_ODR_G_MASK,
                                odr << LSM9DS1_REG_CTRL_REG1_ODR_G_SHIFT);
}

static const struct {
  int freq_int;
  int freq_micros;
} lsm9ds1_gyro_odr_table[] = {{14, 900000}, {59, 500000}, {119, 0},
                              {238, 0},     {476, 0},     {952, 0}};

static int lsm9ds1_set_gyro_odr(const struct device *dev,
                                const struct sensor_value *odr) {
  uint8_t i;

  for (i = 1U; i < ARRAY_SIZE(lsm9ds1_gyro_odr_table); ++i) {
    if (odr->val1 < lsm9ds1_gyro_odr_table[i].freq_int ||
        (odr->val1 == lsm9ds1_gyro_odr_table[i].freq_int &&
         odr->val2 <= lsm9ds1_gyro_odr_table[i].freq_micros)) {
      return lsm9ds1_set_odr_raw(dev, i);
    }
  }

  return -ENOTSUP;
}

static int lsm9ds1_set_accel_fs_raw(const struct device *dev, uint8_t fs) {
  const struct lsm9ds1_config *config = dev->config;

  if (i2c_reg_update_byte_dt(&config->i2c, LSM9DS1_REG_CTRL_REG6,
                             LSM9DS1_REG_CTRL_REG6_FS_XL_MASK,
                             fs << LSM9DS1_REG_CTRL_REG6_FS_XL_SHIFT) < 0) {
    return -EIO;
  }

  data->accel_fs = fs;

  return 0;
}

static const struct {
  int fs;
} lsm9ds1_accel_fs_table[] = {{2}, {16}, {4}, {8}};

static int lsm9ds1_set_accel_fs(const struct device *dev, int fs) {
  uint8_t i;

  for (i = 1U; i < ARRAY_SIZE(lsm9ds1_accel_fs_table); ++i) {
    if (fs == lsm9ds1_accel_fs_table[i].fs) {
      return lsm9ds1_set_accel_fs_raw(dev, i);
    }
  }

  return -ENOTSUP;
}

static inline int lsm9ds1_set_accel_odr_raw(const struct device *dev,
                                            uint8_t odr) {
  const struct lsm9ds1_config *config = dev->config;

  return i2c_reg_update_byte_dt(&config->i2c, LSM9DS1_REG_CTRL_REG1_G,
                                LSM9DS1_MASK_CTRL_REG1_G_DR,
                                odr << LSM9DS1_SHIFT_CTRL_REG1_G_BW);
}

static const struct {
  int freq;
} lsm9ds1_accel_odr_table[] = {{10}, {50}, {119}, {238}, {476}, {952}};

static int lsm9ds1_set_accel_odr(const struct device *dev, int odr) {
  uint8_t i;

  for (i = 1U; i < ARRAY_SIZE(lsm9ds1_accel_odr_table); ++i) {
    if (odr <= lsm9ds1_accel_ord_table[i].freq) {
      return lsm9ds1_set_accel_odr_raw(dev, i);
    }
  }

  return -ENOTSUP;
}

#endif

static inline int read_i2c(const struct device *dev, const uint8_t reg_addr,
                           uint8_t *buf, const uint32_t num_bytes) {
  const struct lsm9ds1_config *config = dev->config;
  if (i2c_burst_read_dt(&config->i2c, reg_addr, buf, num_bytes) < 0) {
    LOG_DBG("Could not read LSM9DS1 data");
    return -EIO;
  }
  return 0;
}

static inline int read_i2c_byte(const struct device *dev,
                                const uint8_t reg_addr, uint8_t *buf) {
  const struct lsm9ds1_config *config = dev->config;
  if (i2c_reg_read_byte_dt(&config->i2c, reg_addr, buf) < 0) {
    LOG_DBG("Could not read LSM9DS1 data");
    return -EIO;
  }
  return 0;
}

static inline int write_i2c_byte(const struct device *dev,
                                 const uint8_t reg_addr, const uint8_t data) {
  const struct lsm9ds1_config *config = dev->config;
  if (i2c_reg_write_byte_dt(&config->i2c, reg_addr, data) < 0) {
    LOG_DBG("Could not write LSM9DS1 data");
    return -EIO;
  }
  return 0;
}

static int lsm9ds1_sample_fetch(const struct device *dev,
                                enum sensor_channel chan) {
  struct lsm9ds1_data *data = dev->data;

  switch (chan) {
  case SENSOR_CHAN_ALL:
    return read_i2c(dev, LSM9DS1_REG_OUT_X_L_G, (uint8_t *)&data->sample,
                    sizeof(data->sample));
  case SENSOR_CHAN_DIE_TEMP:
    return read_i2c(dev, LSM9DS1_REG_OUT_TEMP_L, (uint8_t *)&data->temp,
                    sizeof(data->temp));
  case SENSOR_CHAN_GYRO_X:
    return read_i2c(dev, LSM9DS1_REG_OUT_X_L_G, (uint8_t *)&data->sample.gyro.x,
                    sizeof(data->sample.gyro.x));
  case SENSOR_CHAN_GYRO_Y:
    return read_i2c(dev, LSM9DS1_REG_OUT_Y_L_G, (uint8_t *)&data->sample.gyro.y,
                    sizeof(data->sample.gyro.y));
  case SENSOR_CHAN_GYRO_Z:
    return read_i2c(dev, LSM9DS1_REG_OUT_Z_L_G, (uint8_t *)&data->sample.gyro.z,
                    sizeof(data->sample.gyro.z));
  case SENSOR_CHAN_GYRO_XYZ:
    return read_i2c(dev, LSM9DS1_REG_OUT_X_L_G, (uint8_t *)&data->sample.gyro,
                    sizeof(data->sample.gyro));

  case SENSOR_CHAN_ACCEL_X:
    return read_i2c(dev, LSM9DS1_REG_OUT_X_L_XL,
                    (uint8_t *)&data->sample.accel.x,
                    sizeof(data->sample.accel.x));
  case SENSOR_CHAN_ACCEL_Y:
    return read_i2c(dev, LSM9DS1_REG_OUT_Y_L_XL,
                    (uint8_t *)&data->sample.accel.y,
                    sizeof(data->sample.accel.y));
  case SENSOR_CHAN_ACCEL_Z:
    return read_i2c(dev, LSM9DS1_REG_OUT_Z_L_XL,
                    (uint8_t *)&data->sample.accel.z,
                    sizeof(data->sample.accel.z));
  case SENSOR_CHAN_ACCEL_XYZ:
    return read_i2c(dev, LSM9DS1_REG_OUT_X_L_XL, (uint8_t *)&data->sample.accel,
                    sizeof(data->sample.accel));
  default:
    return -ENOTSUP;
  }
}

static inline struct sensor_value get_sensor_value(const uint16_t value,
                                                   const int32_t divisor) {
  int64_t val = (int16_t)sys_le16_to_cpu(value);
  int64_t tmp = val * 1000000 / divisor;
  struct sensor_value result = {.val1 = tmp / 1000000, .val2 = tmp % 1000000};
  return result;
}

static inline int get_gyro_divisor(const struct lsm9ds1_data *data) {
#if defined(CONFIG_LSM9DS1_SET_RUNTIME)
  switch (data->gyro_fs) {
  case 0:
    return GYRO_FS_245_LSB_PER_RAD;
  case 1:
    return GYRO_FS_500_LSB_PER_RAD;
  default:
    return GYRO_FS_2000_LSB_PER_RAD;
  }
#elif defined(CONFIG_LSM9DS1_GYRO_FULL_SCALE_245)
  return GYRO_FS_245_LSB_PER_RAD;
#elif defined(CONFIG_LSM9DS1_GYRO_FULL_SCALE_500)
  return GYRO_FS_500_LSB_PER_RAD;
#elif defined(CONFIG_LSM9DS1_GYRO_FULL_SCALE_2000)
  return GYRO_FS_2000_LSB_PER_RAD;
#endif
}

static inline int get_accel_divisor(const struct lsm9ds1_data *data) {
#if defined(CONFIG_LSM9DS1_SET_RUNTIME)
  switch (data->accel_fs) {
  case 0:
    return ACCEL_FS_2_LSB_PER_MS;
  case 1:
    return ACCEL_FS_16_LSB_PER_MS;
  case 2:
    return ACCEL_FS_4_LSB_PER_MS;
  default:
    return ACCEL_FS_8_LSB_PER_MS;
  }
#elif defined(CONFIG_LSM9DS1_ACCEL_FULL_SCALE_2)
  return ACCEL_FS_2_LSB_PER_MS;
#elif defined(CONFIG_LSM9DS1_ACCEL_FULL_SCALE_4)
  return ACCEL_FS_16_LSB_PER_MS;
#elif defined(CONFIG_LSM9DS1_ACCEL_FULL_SCALE_8)
  return ACCEL_FS_4_LSB_PER_MS;
#elif defined(CONFIG_LSM9DS1_ACCEL_FULL_SCALE_16)
  return ACCEL_FS_8_LSB_PER_MS;
#endif
}

static int lsm9ds1_channel_get(const struct device *dev,
                               enum sensor_channel chan,
                               struct sensor_value *val) {
  struct lsm9ds1_data *data = dev->data;
  switch (chan) {
  case SENSOR_CHAN_DIE_TEMP:
    *val = get_sensor_value(data->temp, TEMP_LSB_PER_C);
    break;
  case SENSOR_CHAN_GYRO_X:
    *val = get_sensor_value(data->sample.gyro.x, get_gyro_divisor(data));
    break;
  case SENSOR_CHAN_GYRO_Y:
    *val = get_sensor_value(data->sample.gyro.y, get_gyro_divisor(data));
    break;
  case SENSOR_CHAN_GYRO_Z:
    *val = get_sensor_value(data->sample.gyro.z, get_gyro_divisor(data));
    break;
  case SENSOR_CHAN_GYRO_XYZ:
    *val = get_sensor_value(data->sample.gyro.x, get_gyro_divisor(data));
    *(val + 1) = get_sensor_value(data->sample.gyro.y, get_gyro_divisor(data));
    *(val + 2) = get_sensor_value(data->sample.gyro.z, get_gyro_divisor(data));
    break;

  case SENSOR_CHAN_ACCEL_X:
    *val = get_sensor_value(data->sample.accel.x, get_accel_divisor(data));
    break;
  case SENSOR_CHAN_ACCEL_Y:
    *val = get_sensor_value(data->sample.accel.y, get_accel_divisor(data));
    break;
  case SENSOR_CHAN_ACCEL_Z:
    *val = get_sensor_value(data->sample.accel.z, get_accel_divisor(data));
    break;
  case SENSOR_CHAN_ACCEL_XYZ:
    *val = get_sensor_value(data->sample.accel.x, get_accel_divisor(data));
    *(val + 1) =
        get_sensor_value(data->sample.accel.y, get_accel_divisor(data));
    *(val + 2) =
        get_sensor_value(data->sample.accel.z, get_accel_divisor(data));
    break;
  default:
    return -ENOTSUP;
  }

  return 0;
}

#if defined(LSM9DS1_SET_RUNTIME)
static int lsm9ds1_attr_set(const struct device *dev, enum sensor_channel chan,
                            enum sensor_attribute attr,
                            const struct sensor_value *val) {
  switch (attr) {
  case SENSOR_ATTR_FULL_SCALE:
    switch (chan) {
    case SENSOR_CHANNEL_GYRO_XYZ:
      if (lsm9ds1_set_gyro_fs(dev, val) < 0) {
        LOG_DBG("full-scale value not supported");
        return -EIO;
      }
      break;
    case SENSOR_CHANNEL_ACCEL_XYZ:
      if (lsm9ds1_set_accel_fs(dev, val->val1) < 0) {
        LOG_DBG("full-scale value not supported");
        return -EIO;
      }
      break;
    default:
      return -ENOTSUP;
    }
  case SENSOR_ATTR_SAMPLING_FREQUENCY:
    switch (chan) {
    case SENSOR_CHANNEL_GYRO_XYZ:
      if (lsm9ds1_set_gyro_odr(dev, val) < 0) {
        LOG_DBG("sampling frequency value not supported");
        return -EIO;
      }
      break;
    case SENSOR_CHANNEL_ACCEL_XYZ:
      if (lsm9ds1_set_accel_odr(dev, val->val1) < 0) {
        LOG_DBG("sampling frequency value not supported");
        return -EIO;
      }
      break;
    default:
      return -ENOTSUP;
    }
  default:
    return -ENOTSUP;
  }

  return 0;
}
#endif

static const struct sensor_driver_api lsm9ds1_api_funcs = {
    .sample_fetch = lsm9ds1_sample_fetch,
    .channel_get = lsm9ds1_channel_get,
#if defined(LSM9DS1_SET_RUNTIME)
    .attr_set = lsm9ds1_attr_set,
#endif
#if !defined(CONFIG_LSM9DS1_TRIGGER_NONE)
    .trigger_set = lsm9ds1_trigger_set,
#endif
};

static int lsm9ds1_init_chip(const struct device *dev) {
  uint8_t chip_id;

  if (read_i2c_byte(dev, LSM9DS1_REG_WHO_AM_I, &chip_id) < 0) {
    LOG_ERR("Failed reading chip id");
    return -EIO;
  }
  if (chip_id != LSM9DS1_VAL_WHO_AM_I) {
    LOG_ERR("Invalid chip id 0x%x", chip_id);
    return -EIO;
  }

  if (write_i2c_byte(dev, LSM9DS1_REG_CTRL_REG1,
                     LSM9DS1_DEFAULT_ODR_G | LSM9DS1_DEFAULT_FS_G |
                         LSM9DS1_DEFAULT_BW_G) < 0) {
    LOG_ERR("Failed to write CTRL1");
    return -EIO;
  }

  if (write_i2c_byte(dev, LSM9DS1_REG_CTRL_REG2, LSM9DS1_DEFAULT_OUT_SEL) < 0) {
    LOG_ERR("Failed to write CTRL2");
    return -EIO;
  }

  if (write_i2c_byte(dev, LSM9DS1_REG_CTRL_REG3,
                     LSM9DS1_DEFAULT_HPF_ENABLE | LSM9DS1_DEFAULT_HPF_CUTOFF) <
      0) {
    LOG_ERR("Failed to write CTRL3");
    return -EIO;
  }

  if (write_i2c_byte(dev, LSM9DS1_REG_CTRL_REG6,
                     LSM9DS1_DEFAULT_ODR_XL | LSM9DS1_DEFAULT_FS_XL |
                         LSM9DS1_DEFAULT_BW_FILTER_ENABLE |
                         LSM9DS1_DEFAULT_BW_FILTER_SETTING) < 0) {
    LOG_ERR("Failed to write CTRL6");
    return -EIO;
  }

#ifdef CONFIG_LSM9DS1_SET_RUNTIME
  dev->data.gyro_fs = LSM9DS1_DEFAULT_FS_G >> LSM9DS1_REG_CTRL_REG1_FS_G_SHIFT;
  dev->data.accel_fs =
      LSM9DS1_DEFAULT_FS_XL >> LSM9DS1_REG_CTRL_REG6_FS_XL_SHIFT;
#endif

  return 0;
}

static int lsm9ds1_init(const struct device *dev) {
  const struct lsm9ds1_config *const config = dev->config;

  if (!device_is_ready(config->i2c.bus)) {
    LOG_ERR("I2C bus device not ready");
    return -ENODEV;
  }

  if (lsm9ds1_init_chip(dev) < 0) {
    LOG_DBG("failed to initialize chip");
    return -EIO;
  }

#if !defined(CONFIG_LSM9DS1_TRIGGER_NONE)
  if (config->int_gpio.port) {
    if (lsm9ds1_init_interrupt(dev) < 0) {
      LOG_DBG("failed to initialize interrupts");
      return -EIO;
    }
  }
#endif

  return 0;
}

#define LSM9DS1_AG_DEFINE(inst)                                                   \
  static struct lsm9ds1_data lsm9ds1_data_##inst;                              \
                                                                               \
  static const struct lsm9ds1_config lsm9ds1_config_##inst = {                 \
      .i2c = I2C_DT_SPEC_INST_GET(inst),                                       \
      IF_ENABLED(                                                              \
          CONFIG_LSM9DS1_TRIGGER,                                              \
          (.int_gpio = GPIO_DT_SPEC_INST_GET_OR(inst, irq_gpios, {0}), ))};    \
                                                                               \
  SENSOR_DEVICE_DT_INST_DEFINE(                                                \
      inst, lsm9ds1_init, NULL, &lsm9ds1_data_##inst, &lsm9ds1_config_##inst,  \
      POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY, &lsm9ds1_api_funcs);

DT_INST_FOREACH_STATUS_OKAY(LSM9DS1_AG_DEFINE)
