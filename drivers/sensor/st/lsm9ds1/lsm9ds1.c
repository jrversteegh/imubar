/* lsm9ds1.c - Driver for LSM9DS1 accelerometer and gyroscope sensor */

/*
 * Copyright (c) 2024 J.R. Versteegh
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT st_lsm9ds1

#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/__assert.h>
#include <zephyr/sys/byteorder.h>

#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

#include "lsm9ds1.h"

LOG_MODULE_REGISTER(LSM9DS1, CONFIG_SENSOR_LOG_LEVEL);

#define DEG2RAD 0.01745329252f

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

static int lsm9ds1_sample_fetch(const struct device *dev,
                                enum sensor_channel chan) {
  struct lsm9ds1_data *data = dev->data;
  const struct lsm9ds1_config *config = dev->config;

  if (i2c_reg_read_byte_dt(&config->i2c, LSM9DS1_REG_OUT_X_L_G, &x_l) < 0 ||
      i2c_reg_read_byte_dt(&config->i2c, LSM9DS1_REG_OUT_X_H_G, &x_h) < 0 ||
      i2c_reg_read_byte_dt(&config->i2c, LSM9DS1_REG_OUT_Y_L_G, &y_l) < 0 ||
      i2c_reg_read_byte_dt(&config->i2c, LSM9DS1_REG_OUT_Y_H_G, &y_h) < 0 ||
      i2c_reg_read_byte_dt(&config->i2c, LSM9DS1_REG_OUT_Z_L_G, &z_l) < 0 ||
      i2c_reg_read_byte_dt(&config->i2c, LSM9DS1_REG_OUT_Z_H_G, &z_h) < 0) {
    LOG_DBG("failed to read sample");
    return -EIO;
  }

  data->sample_x = (int16_t)((uint16_t)(x_l) | ((uint16_t)(x_h) << 8));
  data->sample_y = (int16_t)((uint16_t)(y_l) | ((uint16_t)(y_h) << 8));
  data->sample_z = (int16_t)((uint16_t)(z_l) | ((uint16_t)(z_h) << 8));

  return 0;
}

static inline void lsm9ds1_convert(struct sensor_value *val, int raw_val,
                                   float scale) {
  float dval;

  dval = (float)(raw_val)*scale;
  val->val1 = (int32_t)dval;
  val->val2 = ((int32_t)(dval * 1000000)) % 1000000;
}

static inline int lsm9ds1_get_channel(enum sensor_channel chan,
                                      struct sensor_value *val,
                                      struct lsm9ds1_data *data,
                                      float numerator) {
  switch (chan) {
  case SENSOR_CHAN_TEMP:
    lsm9ds1_convert(val, data->temp, 0.0625f);
  break case SENSOR_CHAN_GYRO_X:
    lsm9ds1_gyro_convert(val, data->gyro_x, numerator);
    break;
  case SENSOR_CHAN_GYRO_Y:
    lsm9ds1_gyro_convert(val, data->gyro_y, numerator);
    break;
  case SENSOR_CHAN_GYRO_Z:
    lsm9ds1_gyro_convert(val, data->gyro_z, numerator);
    break;
  case SENSOR_CHAN_GYRO_XYZ:
    lsm9ds1_gyro_convert(val, data->sample_x, numerator);
    lsm9ds1_gyro_convert(val + 1, data->sample_y, numerator);
    lsm9ds1_gyro_convert(val + 2, data->sample_z, numerator);
    break;
  default:
    return -ENOTSUP;
  }

  return 0;
}

static int lsm9ds1_channel_get(const struct device *dev,
                               enum sensor_channel chan,
                               struct sensor_value *val) {
  struct lsm9ds1_data *data = dev->data;

#if defined(CONFIG_LSM9DS1_SET_RUNTIME)
  switch (data->gyro_fs) {
  case 0:
    return lsm9ds1_get_channel(chan, val, data, 8.75f);
  case 1:
    return lsm9ds1_get_channel(chan, val, data, 17.50f);
  default:
    return lsm9ds1_get_channel(chan, val, data, 70.0f);
  }
#elif defined(CONFIG_LSM9DS1_FULLSCALE_245)
  return lsm9ds1_get_channel(chan, val, data, 8.75f);
#elif defined(CONFIG_LSM9DS1_FULLSCALE_500)
  return lsm9ds1_get_channel(chan, val, data, 17.50f);
#elif defined(CONFIG_LSM9DS1_FULLSCALE_2000)
  return lsm9ds1_get_channel(chan, val, data, 70.0f);
#endif
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
  const struct lsm9ds1_config *config = dev->config;
  uint8_t chip_id;

  if (i2c_reg_read_byte_dt(&config->i2c, LSM9DS1_REG_WHO_AM_I, &chip_id) < 0) {
    LOG_DBG("failed reading chip id");
    return -EIO;
  }
  if (chip_id != LSM9DS1_VAL_WHO_AM_I) {
    LOG_DBG("invalid chip id 0x%x", chip_id);
    return -EIO;
  }
  LOG_DBG("chip id 0x%x", chip_id);

  if (i2c_reg_update_byte_dt(&config->i2c, LSM9DS1_REG_CTRL_REG4_G,
                             LSM9DS1_MASK_CTRL_REG4_G_BDU |
                                 LSM9DS1_MASK_CTRL_REG4_G_BLE,
                             (1 << LSM9DS1_SHIFT_CTRL_REG4_G_BDU) |
                                 (0 << LSM9DS1_SHIFT_CTRL_REG4_G_BLE)) < 0) {
    LOG_DBG("failed to set BDU and BLE");
    return -EIO;
  }

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

#define LSM9DS1_DEFINE(inst)                                                   \
  static struct lsm9ds1_data lsm9ds1_data_##inst;                              \
                                                                               \
  static const struct lsm9ds1_config lsm9ds1_config_##inst = {                 \
      .i2c = I2C_DT_SPEC_INST_GET(inst),                                       \
      IF_ENABLED(                                                              \
          CONFIG_LSM9DS1_TRIGGER_DRDY,                                         \
          (.int_gpio = GPIO_DT_SPEC_INST_GET_OR(inst, irq_gpios, {0}), ))};    \
                                                                               \
  SENSOR_DEVICE_DT_INST_DEFINE(                                                \
      inst, lsm9ds1_init, NULL, &lsm9ds1_data_##inst, &lsm9ds1_config_##inst,  \
      POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY, &lsm9ds1_api_funcs);

DT_INST_FOREACH_STATUS_OKAY(LSM9DS1_DEFINE)
