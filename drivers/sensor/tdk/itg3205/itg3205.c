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

LOG_MODULE_REGISTER(ITG3205, CONFIG_SENSOR_LOG_LEVEL);

/* see "Temperature Measurement" section from register map description */
static inline void itg3205_convert_temp(enum itg3205_device_type device_type,
                                        struct sensor_value *val,
                                        int16_t raw_val) {
  int64_t tmp_val = (int64_t)raw_val * 1000000;

  switch (device_type) {
  case DEVICE_TYPE_MPU6500:
    tmp_val = (tmp_val * 1000 / 333870) + 21000000;
    break;

  case DEVICE_TYPE_ITG3205:
  default:
    tmp_val = (tmp_val / 340) + 36000000;
  };

  val->val1 = tmp_val / 1000000;
  val->val2 = tmp_val % 1000000;
}

static int itg3205_channel_get(const struct device *dev,
                               enum sensor_channel chan,
                               struct sensor_value *val) {
  struct itg3205_data *drv_data = dev->data;

  switch (chan) {
  case SENSOR_CHAN_ACCEL_XYZ:
    itg3205_convert_accel(val, drv_data->accel_x,
                          drv_data->accel_sensitivity_shift);
    itg3205_convert_accel(val + 1, drv_data->accel_y,
                          drv_data->accel_sensitivity_shift);
    itg3205_convert_accel(val + 2, drv_data->accel_z,
                          drv_data->accel_sensitivity_shift);
    break;
  case SENSOR_CHAN_ACCEL_X:
    itg3205_convert_accel(val, drv_data->accel_x,
                          drv_data->accel_sensitivity_shift);
    break;
  case SENSOR_CHAN_ACCEL_Y:
    itg3205_convert_accel(val, drv_data->accel_y,
                          drv_data->accel_sensitivity_shift);
    break;
  case SENSOR_CHAN_ACCEL_Z:
    itg3205_convert_accel(val, drv_data->accel_z,
                          drv_data->accel_sensitivity_shift);
    break;
  case SENSOR_CHAN_GYRO_XYZ:
    itg3205_convert_gyro(val, drv_data->gyro_x, drv_data->gyro_sensitivity_x10);
    itg3205_convert_gyro(val + 1, drv_data->gyro_y,
                         drv_data->gyro_sensitivity_x10);
    itg3205_convert_gyro(val + 2, drv_data->gyro_z,
                         drv_data->gyro_sensitivity_x10);
    break;
  case SENSOR_CHAN_GYRO_X:
    itg3205_convert_gyro(val, drv_data->gyro_x, drv_data->gyro_sensitivity_x10);
    break;
  case SENSOR_CHAN_GYRO_Y:
    itg3205_convert_gyro(val, drv_data->gyro_y, drv_data->gyro_sensitivity_x10);
    break;
  case SENSOR_CHAN_GYRO_Z:
    itg3205_convert_gyro(val, drv_data->gyro_z, drv_data->gyro_sensitivity_x10);
    break;
  case SENSOR_CHAN_DIE_TEMP:
    itg3205_convert_temp(drv_data->device_type, val, drv_data->temp);
    break;
  default:
    return -ENOTSUP;
  }

  return 0;
}

static int itg3205_sample_fetch(const struct device *dev,
                                enum sensor_channel chan) {
  struct itg3205_data *drv_data = dev->data;
  const struct itg3205_config *cfg = dev->config;
  int16_t buf[7];

  if (i2c_burst_read_dt(&cfg->i2c, ITG3205_REG_DATA_START, (uint8_t *)buf, 14) <
      0) {
    LOG_ERR("Failed to read data sample.");
    return -EIO;
  }

  drv_data->accel_x = sys_be16_to_cpu(buf[0]);
  drv_data->accel_y = sys_be16_to_cpu(buf[1]);
  drv_data->accel_z = sys_be16_to_cpu(buf[2]);
  drv_data->temp = sys_be16_to_cpu(buf[3]);
  drv_data->gyro_x = sys_be16_to_cpu(buf[4]);
  drv_data->gyro_y = sys_be16_to_cpu(buf[5]);
  drv_data->gyro_z = sys_be16_to_cpu(buf[6]);

  return 0;
}

static const struct sensor_driver_api itg3205_driver_api = {
    .sample_fetch = itg3205_sample_fetch,
    .channel_get = itg3205_channel_get,
};

int itg3205_init(const struct device *dev) {
  struct itg3205_data *drv_data = dev->data;
  const struct itg3205_config *cfg = dev->config;
  uint8_t id, i;

  if (!device_is_ready(cfg->i2c.bus)) {
    LOG_ERR("Bus device is not ready");
    return -ENODEV;
  }

  if (i2c_reg_read_byte_dt(&cfg->i2c, ITG3205_REG_CHIP_ID, &id) < 0) {
    LOG_ERR("Failed to read chip ID.");
    return -EIO;
  }

  if (id != ITG3205_CHIP_ID) {
    LOG_DBG("Unexpected chip id (%x)", id);
    return -EIO;
  }

  if (i2c_reg_write_byte_dt(&cfg->i2c, ITG3205_REG_BW,
                            ITG3205_BW << ITG_3205_BW_SHIFT) < 0) {
    LOG_ERR("Failed to set filter bandwith.");
    return -EIO;
  }

  if (i2c_reg_write_byte_dt(&cfg->i2c, ITG3205_REG_SR_DIVIDER,
                            CONFIG_ITG3205_SAMPLE_RATE_DIVIDER) < 0) {
    LOG_ERR("Failed to write sample rate divider");
    return -EIO;
  }

  // Select gyro clock instead of internal oscillator as per datasheet
  // recommendation
  if (i2c_reg_write_byte_dt(&cfg->i2c, ITG3205_REG_PWRMGMT, ITG3205_CLK_SEL) <
      0) {
    LOG_ERR("Failed to set CLK_SEL.");
    return -EIO;
  }

  return 0;
}

#define ITG3205_DEFINE(inst)                                                   \
  static struct itg3205_data itg3205_data_##inst;                              \
                                                                               \
  static const struct itg3205_config itg3205_config_##inst = {                 \
      .i2c = I2C_DT_SPEC_INST_GET(inst),                                       \
  };                                                                           \
                                                                               \
  SENSOR_DEVICE_DT_INST_DEFINE(                                                \
      inst, itg3205_init, NULL, &itg3205_data_##inst, &itg3205_config_##inst,  \
      POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY, &itg3205_driver_api);

DT_INST_FOREACH_STATUS_OKAY(ITG3205_DEFINE)
