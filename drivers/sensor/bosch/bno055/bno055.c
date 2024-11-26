/*
 * Copyright (c) 2024 J.R. Versteegh
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT bosch_bno055

#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/__assert.h>
#include <zephyr/sys/byteorder.h>

#include <assert.h>

#include "bno055.h"

LOG_MODULE_REGISTER(BNO055, CONFIG_SENSOR_LOG_LEVEL);

static int read_i2c(const struct device *dev, const uint8_t reg_addr,
                    uint8_t *buf, const uint32_t num_bytes) {
  const struct bno055_config *config = dev->config;
  if (i2c_burst_read_dt(&config->i2c, reg_addr, buf, num_bytes) < 0) {
    LOG_DBG("Could not read BNO055 data");
    return -EIO;
  }
  return 0;
}

static int read_i2c_byte(const struct device *dev, const uint8_t reg_addr,
                         uint8_t *buf) {
  const struct bno055_config *config = dev->config;
  if (i2c_reg_read_byte_dt(&config->i2c, reg_addr, buf) < 0) {
    LOG_DBG("Could not read BNO055 data");
    return -EIO;
  }
  return 0;
}

static int write_i2c_byte(const struct device *dev, const uint8_t page,
                          const uint8_t reg_addr, const uint8_t data) {
  const struct bno055_config *config = dev->config;
  static uint8_t addr_page = 0;
  if (page != addr_page) {
    if (i2c_reg_write_byte_dt(&config->i2c, BNO055_PAGE_ID_ADDR, page) < 0) {
      LOG_DBG("Could not write BNO055 data");
      return -EIO;
    }
    addr_page = page;
  }
  if (i2c_reg_write_byte_dt(&config->i2c, reg_addr, data) < 0) {
    LOG_DBG("Could not write BNO055 data");
    return -EIO;
  }
  return 0;
}

static int bno055_sample_fetch(const struct device *dev,
                               const enum sensor_channel chan) {
  struct bno055_data *data = dev->data;

  switch (chan) {
  case SENSOR_CHAN_ALL:
    return read_i2c(dev, BNO055_ACCEL_DATA_X_LSB_ADDR, (uint8_t *)&data->sample,
                    sizeof(data->sample));
    // Don't read temperature with "ALL". That should be done explicitly since
    // we do not care about it much
  case SENSOR_CHAN_ACCEL_X:
    return read_i2c(dev, BNO055_ACCEL_DATA_X_LSB_ADDR,
                    (uint8_t *)&data->sample.accel.x,
                    sizeof(data->sample.accel.x));
  case SENSOR_CHAN_ACCEL_Y:
    return read_i2c(dev, BNO055_ACCEL_DATA_Y_LSB_ADDR,
                    (uint8_t *)&data->sample.accel.y,
                    sizeof(data->sample.accel.y));
  case SENSOR_CHAN_ACCEL_Z:
    return read_i2c(dev, BNO055_ACCEL_DATA_Z_LSB_ADDR,
                    (uint8_t *)&data->sample.accel.z,
                    sizeof(data->sample.accel.z));
  case SENSOR_CHAN_ACCEL_XYZ:
    return read_i2c(dev, BNO055_ACCEL_DATA_X_LSB_ADDR,
                    (uint8_t *)&data->sample.accel, sizeof(data->sample.accel));
  case SENSOR_CHAN_GYRO_X:
    return read_i2c(dev, BNO055_GYRO_DATA_X_LSB_ADDR,
                    (uint8_t *)&data->sample.gyro.x,
                    sizeof(data->sample.gyro.x));
  case SENSOR_CHAN_GYRO_Y:
    return read_i2c(dev, BNO055_GYRO_DATA_Y_LSB_ADDR,
                    (uint8_t *)&data->sample.gyro.y,
                    sizeof(data->sample.gyro.y));
  case SENSOR_CHAN_GYRO_Z:
    return read_i2c(dev, BNO055_GYRO_DATA_Z_LSB_ADDR,
                    (uint8_t *)&data->sample.gyro.z,
                    sizeof(data->sample.gyro.z));
  case SENSOR_CHAN_GYRO_XYZ:
    return read_i2c(dev, BNO055_GYRO_DATA_X_LSB_ADDR,
                    (uint8_t *)&data->sample.gyro, sizeof(data->sample.gyro));
  case SENSOR_CHAN_MAGN_X:
    return read_i2c(dev, BNO055_MAGN_DATA_X_LSB_ADDR,
                    (uint8_t *)&data->sample.magn.x,
                    sizeof(data->sample.magn.x));
  case SENSOR_CHAN_MAGN_Y:
    return read_i2c(dev, BNO055_MAGN_DATA_Y_LSB_ADDR,
                    (uint8_t *)&data->sample.magn.y,
                    sizeof(data->sample.magn.y));
  case SENSOR_CHAN_MAGN_Z:
    return read_i2c(dev, BNO055_MAGN_DATA_Z_LSB_ADDR,
                    (uint8_t *)&data->sample.magn.z,
                    sizeof(data->sample.magn.z));
  case SENSOR_CHAN_MAGN_XYZ:
    return read_i2c(dev, BNO055_MAGN_DATA_X_LSB_ADDR,
                    (uint8_t *)&data->sample.magn, sizeof(data->sample.magn));
  case SENSOR_CHAN_DIE_TEMP:
    return read_i2c_byte(dev, BNO055_TEMP_ADDR, (uint8_t *)&data->temp);
  default:
    LOG_DBG("Unsupported sensor channel");
    return -EIO;
  }

  return 0;
}

static struct sensor_value get_sensor_value(const uint16_t value,
                                            const int32_t divisor) {
  int64_t val = (int16_t)sys_le16_to_cpu(value);
  int64_t tmp = val * 1000000 / divisor;
  struct sensor_value result = {.val1 = tmp / 1000000, .val2 = tmp % 1000000};
  return result;
}

static void get_acceleration(const struct device *dev, const uint16_t accel,
                             struct sensor_value *val) {
  *val = get_sensor_value(accel, BNO055_ACCEL_LSB_PER_MS2);
}

static void get_acceleration_vec(const struct device *dev,
                                 const bno055_vec accel,
                                 struct sensor_value *val) {
  for (int i = 0; i < 3; ++i) {
    *val = get_sensor_value(accel.components[i], BNO055_ACCEL_LSB_PER_MS2);
    ++val;
  }
}

static void get_gyro(const struct device *dev, const uint16_t gyro,
                     struct sensor_value *val) {
  struct bno055_data *data = dev->data;
  *val = get_sensor_value(gyro, BNO055_GYRO_LSB_PER_RPS * data->gyr_div);
}

static void get_gyro_vec(const struct device *dev, const bno055_vec gyro,
                         struct sensor_value *val) {
  struct bno055_data *data = dev->data;
  for (int i = 0; i < 3; ++i) {
    *val = get_sensor_value(gyro.components[i],
                            BNO055_GYRO_LSB_PER_RPS * data->gyr_div);
    ++val;
  }
}

static void get_magn(const struct device *dev, const uint16_t magn,
                     struct sensor_value *val) {
  *val = get_sensor_value(magn, BNO055_MAGN_LSB_PER_GS);
}

static void get_magn_vec(const struct device *dev, const bno055_vec magn,
                         struct sensor_value *val) {
  for (int i = 0; i < 3; ++i) {
    *val = get_sensor_value(magn.components[i], BNO055_MAGN_LSB_PER_GS);
    ++val;
  }
}

static int set_operation_mode(const struct device *dev, const uint8_t mode) {
  static uint8_t current_mode = 0;
  static bool initialized = false;
  int result = 0;
  if (mode != current_mode || !initialized) {
    result = write_i2c_byte(dev, 0, BNO055_OPR_MODE_ADDR, mode);
    if (result == 0) {
      current_mode = mode;
      initialized = true;
    }
    k_sleep(K_MSEC(25));
  }
  return result;
}

static int set_unit_selection(const struct device *dev) {
  int result = 0;
  if ((result = set_operation_mode(dev, operation_mode_config)) != 0) {
    return result;
  };
  union unit_sel usel = {.config = {.angle_order = angle_order_android,
                                    .temperature = temperature_celcius,
                                    .euler_angles = euler_angles_radians,
                                    .angular_rate = angular_rate_rps,
                                    .acceleration = acceleration_ms2}};
  if (usel.byte != 134)
    return -ENOTSUP;
  return write_i2c_byte(dev, 0, BNO055_UNIT_SEL_ADDR, usel.byte);
}

static int set_accel_config(const struct device *dev) {
  int result = 0;
  if ((result = set_operation_mode(dev, operation_mode_config)) != 0) {
    return result;
  };
  union accel_config a_conf = {
      .config = {
#ifdef CONFIG_BNO055_ACCEL_RANGE_2G
          .accel_range = accel_config_range_2g,
#elif defined CONFIG_BNO055_ACCEL_RANGE_4G
          .accel_range = accel_config_range_4g,
#elif defined CONFIG_BNO055_ACCEL_RANGE_8G
          .accel_range = accel_config_range_8g,
#elif defined CONFIG_BNO055_ACCEL_RANGE_16G
          .accel_range = accel_config_range_16g,
#else
#error "No accel range selected"
#endif
#ifdef CONFIG_BNO055_ACCEL_BANDWIDTH_8HZ
          .accel_bandwidth = accel_config_bandwidth_8Hz,
#elif defined CONFIG_BNO055_ACCEL_BANDWIDTH_16HZ
          .accel_bandwidth = accel_config_bandwidth_16Hz,
#elif defined CONFIG_BNO055_ACCEL_BANDWIDTH_32HZ
          .accel_bandwidth = accel_config_bandwidth_32Hz,
#elif defined CONFIG_BNO055_ACCEL_BANDWIDTH_63HZ
          .accel_bandwidth = accel_config_bandwidth_63Hz,
#elif defined CONFIG_BNO055_ACCEL_BANDWIDTH_125HZ
          .accel_bandwidth = accel_config_bandwidth_125Hz,
#elif defined CONFIG_BNO055_ACCEL_BANDWIDTH_250HZ
          .accel_bandwidth = accel_config_bandwidth_250Hz,
#elif defined CONFIG_BNO055_ACCEL_BANDWIDTH_500HZ
          .accel_bandwidth = accel_config_bandwidth_500Hz,
#elif defined CONFIG_BNO055_ACCEL_BANDWIDTH_1000HZ
          .accel_bandwidth = accel_config_bandwidth_1000Hz,
#else
#error "No accel bandwidth selected"
#endif
          .accel_power_mode = accel_config_power_mode_normal,
      }};
  return write_i2c_byte(dev, 1, BNO055_ACCEL_CONFIG_ADDR, a_conf.byte);
}

static int set_gyro_config(const struct device *dev) {
  int result = 0;
  struct bno055_data *data = dev->data;
  if ((result = set_operation_mode(dev, operation_mode_config)) != 0) {
    return result;
  };
#ifdef CONFIG_BNO055_GYRO_RANGE_125DPS
  data->gyr_div = 16;
#elif defined CONFIG_BNO055_GYRO_RANGE_250DPS
  data->gyr_div = 8;
#elif defined CONFIG_BNO055_GYRO_RANGE_500DPS
  data->gyr_div = 4;
#elif defined CONFIG_BNO055_GYRO_RANGE_1000DPS
  data->gyr_div = 2;
#elif defined CONFIG_BNO055_GYRO_RANGE_2000DPS
  data->gyr_div = 1;
#else
#error "No gyro range selected"
#endif
  union gyro_config g_conf = {.config = {
#ifdef CONFIG_BNO055_GYRO_RANGE_125DPS
                                  .gyro_range = gyro_mode_range_125dps,
#elif defined CONFIG_BNO055_GYRO_RANGE_250DPS
                                  .gyro_range = gyro_mode_range_250dps,
#elif defined CONFIG_BNO055_GYRO_RANGE_500DPS
                                  .gyro_range = gyro_mode_range_500dps,
#elif defined CONFIG_BNO055_GYRO_RANGE_1000DPS
                                  .gyro_range = gyro_mode_range_1000dps,
#elif defined CONFIG_BNO055_GYRO_RANGE_2000DPS
                                  .gyro_range = gyro_mode_range_2000dps,
#else
#error "No gyro range selected"
#endif
#ifdef CONFIG_BNO055_GYRO_BANDWIDTH_12HZ
                                  .gyro_bandwidth = gyro_mode_bandwidth_12Hz,
#elif defined CONFIG_BNO055_GYRO_BANDWIDTH_23HZ
                                  .gyro_bandwidth = gyro_mode_bandwidth_23Hz,
#elif defined CONFIG_BNO055_GYRO_BANDWIDTH_32HZ
                                  .gyro_bandwidth = gyro_mode_bandwidth_32Hz,
#elif defined CONFIG_BNO055_GYRO_BANDWIDTH_47HZ
                                  .gyro_bandwidth = gyro_mode_bandwidth_47Hz,
#elif defined CONFIG_BNO055_GYRO_BANDWIDTH_64HZ
                                  .gyro_bandwidth = gyro_mode_bandwidth_64Hz,
#elif defined CONFIG_BNO055_GYRO_BANDWIDTH_116HZ
                                  .gyro_bandwidth = gyro_mode_bandwidth_116Hz,
#elif defined CONFIG_BNO055_GYRO_BANDWIDTH_230HZ
                                  .gyro_bandwidth = gyro_mode_bandwidth_230Hz,
#elif defined CONFIG_BNO055_GYRO_BANDWIDTH_523HZ
                                  .gyro_bandwidth = gyro_mode_bandwidth_523Hz,
#else
#error "No gyro bandwidth selected"
#endif
                              }};
  return write_i2c_byte(dev, 1, BNO055_GYRO_CONFIG_ADDR, g_conf.byte);
}

static int set_magn_config(const struct device *dev) {
  int result = 0;
  if ((result = set_operation_mode(dev, operation_mode_config)) != 0) {
    return result;
  };
  union magn_config m_conf = {
      .config = {
#ifdef CONFIG_BNO055_MAGN_DATA_RATE_2HZ
          .magn_data_rate = magn_config_data_rate_2Hz,
#elif defined CONFIG_BNO055_MAGN_DATA_RATE_6HZ
          .magn_data_rate = magn_config_data_rate_6Hz,
#elif defined CONFIG_BNO055_MAGN_DATA_RATE_8HZ
          .magn_data_rate = magn_config_data_rate_8Hz,
#elif defined CONFIG_BNO055_MAGN_DATA_RATE_10HZ
          .magn_data_rate = magn_config_data_rate_10Hz,
#elif defined CONFIG_BNO055_MAGN_DATA_RATE_15HZ
          .magn_data_rate = magn_config_data_rate_15Hz,
#elif defined CONFIG_BNO055_MAGN_DATA_RATE_20HZ
          .magn_data_rate = magn_config_data_rate_20Hz,
#elif defined CONFIG_BNO055_MAGN_DATA_RATE_25HZ
          .magn_data_rate = magn_config_data_rate_25Hz,
#elif defined CONFIG_BNO055_MAGN_DATA_RATE_30HZ
          .magn_data_rate = magn_config_data_rate_30Hz,
#else
#error "Magn data rate not defined"
#endif
#ifdef CONFIG_BNO055_MAGN_OPERATION_MODE_LOW_POWER
          .magn_operation_mode = magn_config_operation_low_power,
#elif defined CONFIG_BNO055_MAGN_OPERATION_MODE_REGULAR
          .magn_operation_mode = magn_config_operation_regular,
#elif defined CONFIG_BNO055_MAGN_OPERATION_MODE_ENHANCED_REGULAR
          .magn_operation_mode = magn_config_operation_enhanced,
#elif defined CONFIG_BNO055_MAGN_OPERATION_MODE_HIGH_ACCURACY
          .magn_operation_mode = magn_config_operation_high_accuracy,
#else
#error "Magn operation mode not defined"
#endif
#ifdef CONFIG_BNO055_MAGN_POWER_MODE_NORMAL
          .magn_power_mode = magn_config_power_mode_normal,
#elif defined CONFIG_BNO055_MAGN_POWER_MODE_SLEEP
          .magn_power_mode = magn_config_power_mode_sleep,
#elif defined CONFIG_BNO055_MAGN_POWER_MODE_SUSPEND
          .magn_power_mode = magn_config_power_mode_suspend,
#elif defined CONFIG_BNO055_MAGN_POWER_MODE_FORCE_MODE
          .magn_power_mode = magn_config_power_mode_force,
#else
#error "Magn power mode not defined"
#endif
      }};
  return write_i2c_byte(dev, 1, BNO055_MAGN_CONFIG_ADDR, m_conf.byte);
}

static int bno055_channel_get(const struct device *dev,
                              enum sensor_channel chan,
                              struct sensor_value *val) {
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
                           const struct sensor_value *val) {
  return -ENOTSUP;
}

static int bno055_attr_get(const struct device *dev, enum sensor_channel chan,
                           enum sensor_attribute attr,
                           struct sensor_value *val) {
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

int bno055_init(const struct device *dev) {
  const struct bno055_config *config = dev->config;
  uint8_t id = 0U;
  int result = 0;

  /* Sensor can take time to boot up wait a second before initializing */
  k_sleep(K_MSEC(1000));

  if (!device_is_ready(config->i2c.bus)) {
    LOG_ERR("I2C bus device not ready");
    return -ENODEV;
  }

  /* read device ID */
  if (i2c_reg_read_byte_dt(&config->i2c, BNO055_CHIP_ID_ADDR, &id) < 0) {
    LOG_DBG("Could not read chip id");
    return -EIO;
  }

  if (id != BNO055_CHIP_ID) {
    LOG_DBG("Unexpected chip id (%x)", id);
    return -EIO;
  }

  set_operation_mode(dev, operation_mode_config);

  if ((result = set_unit_selection(dev)) != 0) {
    LOG_DBG("Failed to set unit selection");
    return result;
  }

  if ((result = set_accel_config(dev)) != 0) {
    LOG_DBG("Failed to accelerometer configuration");
    return result;
  }

  if ((result = set_gyro_config(dev)) != 0) {
    LOG_DBG("Failed to configure gyro");
    return result;
  }

  if ((result = set_magn_config(dev)) != 0) {
    LOG_DBG("Failed to configure magnetometer");
    return result;
  }

#ifdef CONFIG_BNO055_TRIGGER
  if (config->int1_gpio.port) {
    if (bno055_init_interrupt(dev) < 0) {
      LOG_DBG("Could not initialize interrupts");
      return -EIO;
    }
  }
#endif

#ifdef CONFIG_BNO055_OPERATION_MODE_ACCEL_ONLY
  return set_operation_mode(dev, operation_mode_accel_only);
#elif defined CONFIG_BNO055_OPERATION_MODE_MAGN_ONLY
  return set_operation_mode(dev, operation_mode_magn_only);
#elif defined CONFIG_BNO055_OPERATION_MODE_GYRO_ONLY
  return set_operation_mode(dev, operation_mode_gyro_only);
#elif defined CONFIG_BNO055_OPERATION_MODE_ACCEL_MAGN
  return set_operation_mode(dev, operation_mode_accel_magn);
#elif defined CONFIG_BNO055_OPERATION_MODE_ACCEL_GYRO
  return set_operation_mode(dev, operation_mode_accel_gyro);
#elif defined CONFIG_BNO055_OPERATION_MODE_GYRO_MAGN
  return set_operation_mode(dev, operation_mode_gyro_magn);
#elif defined CONFIG_BNO055_OPERATION_MODE_ACCEL_GYRO_MAGN
  return set_operation_mode(dev, operation_mode_accel_gyro_magn);
#elif defined CONFIG_BNO055_OPERATION_MODE_IMU
  return set_operation_mode(dev, operation_mode_imu);
#elif defined CONFIG_BNO055_OPERATION_MODE_COMPASS
  return set_operation_mode(dev, operation_mode_compass);
#elif defined CONFIG_BNO055_OPERATION_MODE_M4G
  return set_operation_mode(dev, operation_mode_m4g);
#elif defined CONFIG_BNO055_OPERATION_MODE_NDOF_FMC_OFF
  return set_operation_mode(dev, operation_mode_ndof_fmc_off);
#elif defined CONFIG_BNO055_OPERATION_MODE_NDOF
  return set_operation_mode(dev, operation_mode_ndof);
#else
#error "No BNO055 operation mode set"
#endif
}

#if defined(CONFIG_BNO055_TRIGGER)
#define BNO055_TRIGGER_CFG(inst)                                               \
  .interrupt = GPIO_DT_SPEC_INST_GET(inst, int_gpios),
#else
#define BNO055_TRIGGER_CFG(inst)
#endif

#define BNO055_DEFINE(inst)                                                    \
  static struct bno055_data bno055_data_##inst;                                \
                                                                               \
  static const struct bno055_config bno055_config##inst = {                    \
      .i2c = I2C_DT_SPEC_INST_GET(inst), BNO055_TRIGGER_CFG(inst)};            \
                                                                               \
  SENSOR_DEVICE_DT_INST_DEFINE(                                                \
      inst, bno055_init, NULL, &bno055_data_##inst, &bno055_config##inst,      \
      POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY, &bno055_driver_api);

DT_INST_FOREACH_STATUS_OKAY(BNO055_DEFINE)
