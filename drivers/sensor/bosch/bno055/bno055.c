/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT bosch_bno055

#include <zephyr/drivers/i2c.h>
#include <zephyr/init.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/__assert.h>
#include <zephyr/logging/log.h>

#include "bno055.h"


LOG_MODULE_REGISTER(BNO055, CONFIG_SENSOR_LOG_LEVEL);

static int read_i2c(const struct device *dev, const uint8_t reg_addr, uint8_t * buf, const uint32_t num_bytes) {
  const struct bno055_config *config = dev->config;
  if (i2c_burst_read_dt(&config->i2c, reg_addr, buf, num_bytes) < 0) {
    LOG_DBG("Could not BNO055 data");
    return -EIO;
  }
  return 0;
}

static int read_i2c_byte(const struct device *dev, const uint8_t reg_addr, uint8_t * buf) {
  const struct bno055_config *config = dev->config;
  if (i2c_reg_read_byte_dt(&config->i2c, reg_addr, buf) < 0) {
    LOG_DBG("Could not BNO055 data");
    return -EIO;
  }
  return 0;
}

static int bno055_sample_fetch(const struct device *dev,
    const enum sensor_channel chan)
{
  struct bno055_data *data = dev->data;

  switch (chan) {
    case SENSOR_CHAN_ALL:
      return read_i2c(dev, BNO055_ACCEL_DATA_X_LSB_ADDR, (uint8_t*)&data->sample, sizeof(data->sample));
      // Don't read temperature with "ALL". That should be done explicitly since we do not care about it much
    case SENSOR_CHAN_ACCEL_X:
      return read_i2c(dev, BNO055_ACCEL_DATA_X_LSB_ADDR, (uint8_t*)&data->sample.accel.x, sizeof(data->sample.accel.x));
    case SENSOR_CHAN_ACCEL_Y:
      return read_i2c(dev, BNO055_ACCEL_DATA_Y_LSB_ADDR, (uint8_t*)&data->sample.accel.y, sizeof(data->sample.accel.y));
    case SENSOR_CHAN_ACCEL_Z:
      return read_i2c(dev, BNO055_ACCEL_DATA_Z_LSB_ADDR, (uint8_t*)&data->sample.accel.z, sizeof(data->sample.accel.z));
    case SENSOR_CHAN_ACCEL_XYZ:
      return read_i2c(dev, BNO055_ACCEL_DATA_X_LSB_ADDR, (uint8_t*)&data->sample.accel, sizeof(data->sample.accel));
    case SENSOR_CHAN_GYRO_X:
      return read_i2c(dev, BNO055_GYRO_DATA_X_LSB_ADDR, (uint8_t*)&data->sample.gyro.x, sizeof(data->sample.gyro.x));
    case SENSOR_CHAN_GYRO_Y:
      return read_i2c(dev, BNO055_GYRO_DATA_Y_LSB_ADDR, (uint8_t*)&data->sample.gyro.y, sizeof(data->sample.gyro.y));
    case SENSOR_CHAN_GYRO_Z:
      return read_i2c(dev, BNO055_GYRO_DATA_Z_LSB_ADDR, (uint8_t*)&data->sample.gyro.z, sizeof(data->sample.gyro.z));
    case SENSOR_CHAN_GYRO_XYZ:
      return read_i2c(dev, BNO055_GYRO_DATA_X_LSB_ADDR, (uint8_t*)&data->sample.gyro, sizeof(data->sample.gyro));
    case SENSOR_CHAN_MAGN_X:
      return read_i2c(dev, BNO055_MAG_DATA_X_LSB_ADDR, (uint8_t*)&data->sample.magn.x, sizeof(data->sample.magn.x));
    case SENSOR_CHAN_MAGN_Y:
      return read_i2c(dev, BNO055_MAG_DATA_Y_LSB_ADDR, (uint8_t*)&data->sample.magn.y, sizeof(data->sample.magn.y));
    case SENSOR_CHAN_MAGN_Z:
      return read_i2c(dev, BNO055_MAG_DATA_Z_LSB_ADDR, (uint8_t*)&data->sample.magn.z, sizeof(data->sample.magn.z));
    case SENSOR_CHAN_MAGN_XYZ:
      return read_i2c(dev, BNO055_MAG_DATA_X_LSB_ADDR, (uint8_t*)&data->sample.magn, sizeof(data->sample.magn));
    case SENSOR_CHAN_DIE_TEMP:
      return read_i2c_byte(dev, BNO055_TEMP_ADDR, (uint8_t*)&data->temp);
    default:
      LOG_DBG("Unsupported sensor channel");
      return -EIO;
    }

  return 0;
}

struct sensor_value get_sensor_value(const uint16_t value, const int32_t divisor) {
  uint32_t tmp = (uint64_t)value * 1000000 / divisor;
  struct sensor_value result = {
    .val1 = tmp / 1000000,
    .val2 = tmp % 1000000
  };
  return result;
}

static void get_acceleration(const struct device *dev, const uint16_t accel, struct sensor_value *val) {
  *val = get_sensor_value(accel, BNO055_ACCEL_LSB_PER_MS);
}

static void get_acceleration_vec(const struct device *dev, const bno055_vec accel, struct sensor_value *val) {
  for (int i = 0; i < 3; ++i) {
     *val = get_sensor_value(accel.components[i], BNO055_ACCEL_LSB_PER_MS);
     ++val;
  }
}

static void get_gyro(const struct device *dev, const uint16_t gyro, struct sensor_value *val) {
  *val = get_sensor_value(gyro, BNO055_GYRO_LSB_PER_RPS);
}

static void get_gyro_vec(const struct device *dev, const bno055_vec gyro, struct sensor_value *val) {
  for (int i = 0; i < 3; ++i) {
     *val = get_sensor_value(gyro.components[i], BNO055_GYRO_LSB_PER_RPS);
     ++val;
  }
}

static void get_magn(const struct device *dev, const uint16_t magn, struct sensor_value *val) {
  *val = get_sensor_value(magn, BNO055_MAGN_LSB_PER_GS);
}

static void get_magn_vec(const struct device *dev, const bno055_vec magn, struct sensor_value *val) {
  for (int i = 0; i < 3; ++i) {
     *val = get_sensor_value(magn.components[i], BNO055_MAGN_LSB_PER_GS);
     ++val;
  }
}


static int bno055_channel_get(const struct device *dev,
    enum sensor_channel chan,
    struct sensor_value *val)
{
  struct bno055_data *data = dev->data;

  switch (chan) {
    case SENSOR_CHAN_ACCEL_X:
      get_acceleration(dev, data->sample.accel.x, val);
      break;
    case SENSOR_CHAN_ACCEL_Y:
      get_acceleration(dev, data->sample.accel.y, val);
      break;
    case SENSOR_CHAN_ACCEL_Z:
      get_acceleration(dev, data->sample.accel.z, val);
      break;
    case SENSOR_CHAN_ACCEL_XYZ:
      get_acceleration_vec(dev, data->sample.accel, val);
      break;
    case SENSOR_CHAN_GYRO_X:
      get_gyro(dev, data->sample.gyro.x, val);
      break;
    case SENSOR_CHAN_GYRO_Y:
      get_gyro(dev, data->sample.gyro.y, val);
      break;
    case SENSOR_CHAN_GYRO_Z:
      get_gyro(dev, data->sample.gyro.z, val);
      break;
    case SENSOR_CHAN_GYRO_XYZ:
      get_gyro_vec(dev, data->sample.gyro, val);
      break;
    case SENSOR_CHAN_MAGN_X:
      get_magn(dev, data->sample.magn.x, val);
      break;
    case SENSOR_CHAN_MAGN_Y:
      get_magn(dev, data->sample.magn.y, val);
      break;
    case SENSOR_CHAN_MAGN_Z:
      get_magn(dev, data->sample.magn.z, val);
      break;
    case SENSOR_CHAN_MAGN_XYZ:
      get_magn_vec(dev, data->sample.magn, val);
      break;
    case SENSOR_CHAN_DIE_TEMP:
      *val = get_sensor_value(data->temp, BNO055_TEMP_LSB_PER_C);
      break;
    default:
      LOG_DBG("Unsupported sensor channel");
      return -ENOTSUP;
  }

  return 0;
}


static int bno055_attr_set(const struct device *dev, enum sensor_channel chan,
                           enum sensor_attribute attr,
                           const struct sensor_value *val)
{
  return -ENOTSUP;
}


static int bno055_attr_get(const struct device *dev, enum sensor_channel chan,
                           enum sensor_attribute attr, struct sensor_value *val)
{
  return -ENOTSUP;
}


static const struct sensor_driver_api bno055_driver_api = {
  .attr_set = bno055_attr_set,
  .attr_get = bno055_attr_get,
#if CONFIG_BNO055_TRIGGER
  .trigger_set = bno055_trigger_set,
#endif
  .sample_fetch = bno055_sample_fetch,
  .channel_get = bno055_channel_get,
};

int bno055_init(const struct device *dev)
{
  const struct bno055_config *config = dev->config;
  uint8_t id = 0U;

  if (!device_is_ready(config->i2c.bus)) {
    LOG_ERR("I2C bus device not ready");
    return -ENODEV;
  }

  /* read device ID */
  if (i2c_reg_read_byte_dt(&config->i2c,
         BNO055_CHIP_ID_ADDR, &id) < 0) {
    LOG_DBG("Could not read chip id");
    return -EIO;
  }

  if (id != BNO055_CHIP_ID) {
    LOG_DBG("Unexpected chip id (%x)", id);
    return -EIO;
  }

#ifdef CONFIG_BNO055_TRIGGER
  if (config->int1_gpio.port) {
    if (bno055_init_interrupt(dev) < 0) {
      LOG_DBG("Could not initialize interrupts");
      return -EIO;
    }
  }
#endif

  return 0;
}

#if defined(CONFIG_BNO055_TRIGGER)
#define BNO055_TRIGGER_CFG(inst) \
        .interrupt = GPIO_DT_SPEC_INST_GET(inst, int_gpios),
#else
#define BNO055_TRIGGER_CFG(inst)
#endif

#define BNO055_DEFINE(inst)                                                  \
  static struct bno055_data bno055_data_##inst;                              \
                                                                             \
  static const struct bno055_config bno055_config##inst = {                  \
    .i2c = I2C_DT_SPEC_INST_GET(inst),                                       \
    BNO055_TRIGGER_CFG(inst)                                                 \
  };                                                                         \
                                                                             \
  SENSOR_DEVICE_DT_INST_DEFINE(inst, bno055_init, NULL, &bno055_data_##inst, \
            &bno055_config##inst, POST_KERNEL,                               \
            CONFIG_SENSOR_INIT_PRIORITY, &bno055_driver_api);

DT_INST_FOREACH_STATUS_OKAY(BNO055_DEFINE)
