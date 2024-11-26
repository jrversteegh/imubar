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
#include <zephyr/sys/byteorder.h>

#include "bmp085.h"

LOG_MODULE_REGISTER(BMP085, CONFIG_SENSOR_LOG_LEVEL);

static int read_i2c(const struct device *dev, const uint8_t reg_addr,
                    uint8_t *buf, const uint32_t num_bytes) {
  const struct bmp085_config *config = dev->config;
  if (i2c_burst_read_dt(&config->i2c, reg_addr, buf, num_bytes) < 0) {
    LOG_DBG("Could not read BMP085 data");
    return -EIO;
  }
  return 0;
}

static int read_i2c_byte(const struct device *dev, const uint8_t reg_addr,
                         uint8_t *buf) {
  const struct bmp085_config *config = dev->config;
  if (i2c_reg_read_byte_dt(&config->i2c, reg_addr, buf) < 0) {
    LOG_DBG("Could not read BMP085 data from %d", (int)reg_addr);
    return -EIO;
  }
  return 0;
}

static int read_i2c_word(const struct device *dev, const uint8_t reg_addr,
                         uint16_t *buf) {
  int r = read_i2c(dev, reg_addr, (uint8_t *)buf, 2);
  if (r < 0)
    return r;

  *buf = sys_be16_to_cpu(*buf);
  return 0;
}

static int write_i2c_byte(const struct device *dev, const uint8_t reg_addr,
                          uint8_t value) {
  const struct bmp085_config *config = dev->config;
  if (i2c_reg_write_byte_dt(&config->i2c, reg_addr, value) < 0) {
    LOG_DBG("Could not write BMP085 data to %d", (int)reg_addr);
    return -EIO;
  }
  return 0;
}

static struct sensor_value get_sensor_value(const int32_t value,
                                            const int32_t divisor) {
  int64_t tmp = (uint64_t)value * 1000000 / divisor;
  struct sensor_value result = {.val1 = tmp / 1000000, .val2 = tmp % 1000000};
  return result;
}

int32_t compute_b5(const struct bmp085_coefficients *c, uint16_t ut) {
  int32_t x1 = ((int32_t)ut - (int32_t)c->ac6) * (int32_t)c->ac5 >> 15;
  int32_t x2 = ((int32_t)c->mc << 11) / (x1 + (int32_t)c->md);
  return x1 + x2;
}

static int bmp085_sample_fetch(const struct device *dev,
                               const enum sensor_channel chan) {
  struct bmp085_data *data = dev->data;
  int ret;
  uint8_t p_buf[3];

  switch (chan) {
  case SENSOR_CHAN_PRESS:
    uint8_t control;
    if ((ret = read_i2c_byte(dev, BMP085_CONTROL, &control)) < 0)
      return ret;
    // Check if the busy bit is set in the control register...
    if (control & 0x20) {
      // ..and return the current state if so
      return data->fetch_state;
    }
    switch (data->fetch_state) {
    case bmp085_fetch_new:
      if ((ret = write_i2c_byte(dev, BMP085_CONTROL, BMP085_READTEMPCMD)) < 0)
        return ret;
      return ++data->fetch_state;
    case bmp085_temperature_commanded:
      if ((ret = read_i2c_word(dev, BMP085_TEMPDATA, &data->temperature)) < 0)
        return ret;
      return ++data->fetch_state;
    case bmp085_temperature_fetched:
      if ((ret = write_i2c_byte(dev, BMP085_CONTROL,
                                BMP085_READPRESSURECMD +
                                    (BMP085_OVERSAMPLING << 6))) < 0)
        return ret;
      return ++data->fetch_state;
    case bmp085_pressure_commanded:
      if ((ret = read_i2c(dev, BMP085_PRESSUREDATA, p_buf, sizeof(p_buf))) < 0)
        return ret;
      data->pressure = (p_buf[0] << 16) + (p_buf[1] << 8) + p_buf[2];
      data->pressure >>= (8 - BMP085_OVERSAMPLING);
      data->fetch_state = bmp085_fetch_new;
      return 0;
    default:
      data->fetch_state = bmp085_fetch_new;
      return 0;
    }
  case SENSOR_CHAN_ALL:
    if ((ret = write_i2c_byte(dev, BMP085_CONTROL, BMP085_READTEMPCMD)) < 0)
      return ret;
    k_sleep(K_MSEC(5));
    if ((ret = read_i2c_word(dev, BMP085_TEMPDATA, &data->temperature)) < 0)
      return ret;

    if ((ret = write_i2c_byte(dev, BMP085_CONTROL,
                              BMP085_READPRESSURECMD +
                                  (BMP085_OVERSAMPLING << 6))) < 0)
      return ret;
    k_sleep(K_MSEC(BMP085_PRESSURE_READ_DELAY));
    if ((ret = read_i2c(dev, BMP085_PRESSUREDATA, p_buf, sizeof(p_buf))) < 0)
      return ret;
    data->pressure = (p_buf[0] << 16) + (p_buf[1] << 8) + p_buf[2];
    data->pressure >>= (8 - BMP085_OVERSAMPLING);
    LOG_DBG("Raw temp: %d  Raw pressure: %d", (int)data->temperature,
            (int)data->pressure);
    return 0;
  default:
    return -ENOTSUP;
  }
}

static int bmp085_channel_get(const struct device *dev,
                              enum sensor_channel chan,
                              struct sensor_value *val) {
  int32_t b3, b5, b6, x1, x2, x3, p;
  uint32_t b4, b7;

  struct bmp085_data *data = dev->data;
  struct bmp085_coefficients *c = &data->calibration.coefficients;
  b5 = compute_b5(c, data->temperature);

  switch (chan) {
  case SENSOR_CHAN_DIE_TEMP:
  case SENSOR_CHAN_AMBIENT_TEMP:
    *val = get_sensor_value(b5 + 8, 160);
    break;
  case SENSOR_CHAN_PRESS:
    b6 = b5 - 4000;
    x1 = (int32_t)c->b2 * (b6 * b6 >> 12) >> 11;
    x2 = (int32_t)c->ac2 * b6 >> 11;
    x3 = x1 + x2;
    b3 = ((((int32_t)c->ac1 * 4 + x3) << BMP085_OVERSAMPLING) + 2) / 4;
    x1 = (int32_t)c->ac3 * b6 >> 13;
    x2 = (int32_t)c->b1 * ((b6 * b6) >> 12) >> 16;
    x3 = (x1 + x2 + 2) >> 2;
    b4 = (uint32_t)c->ac4 * (uint32_t)(x3 + 32768) >> 15;
    b7 = (data->pressure - b3) * (50000 >> BMP085_OVERSAMPLING);
    if (b7 < 0x80000000) {
      p = (b7 * 2) / b4;
    } else {
      p = (b7 / b4) * 2;
    }
    x1 = (p >> 8) * (p >> 8);
    x1 = (x1 * 3038) >> 16;
    x2 = (-7357 * p) >> 16;
    p += (x1 + x2 + 3791) >> 4;
    struct sensor_value result = {.val1 = p, .val2 = 0};
    *val = result;
    break;
  default:
    return -ENOTSUP;
  }
  return 0;
}

int bmp085_init(const struct device *dev) {
  const struct bmp085_config *config = dev->config;
  struct bmp085_data *data = dev->data;

  uint8_t id = 0U;

  if (!device_is_ready(config->i2c.bus)) {
    LOG_ERR("I2C bus device not ready");
    return -ENODEV;
  }

  if (i2c_reg_read_byte_dt(&config->i2c, BMP085_CHIP_ID_ADDR, &id) < 0) {
    LOG_DBG("Could not read chip id");
    return -EIO;
  }

  if (id != BMP085_CHIP_ID) {
    LOG_DBG("Unexpected chip id (%x)", id);
    return -EIO;
  }

  data->fetch_state = bmp085_fetch_new;

  if (i2c_burst_read_dt(&config->i2c, BMP085_CAL_BASE,
                        (uint8_t *)&data->calibration.data,
                        sizeof(data->calibration.data)) < 0) {
    LOG_DBG("Could not read BMP085 calibration data");
    return -EIO;
  }
  // Coefficients are stored in big endian: convert to cpu endianess
  for (int i = 0; i < NUM_CAL_COEFFS; ++i) {
    data->calibration.data[i] = sys_be16_to_cpu(data->calibration.data[i]);
  }

  LOG_DBG("ac1 = %d", (int)data->calibration.coefficients.ac1);
  LOG_DBG("ac2 = %d", (int)data->calibration.coefficients.ac2);
  LOG_DBG("ac3 = %d", (int)data->calibration.coefficients.ac3);
  LOG_DBG("ac4 = %d", (int)data->calibration.coefficients.ac4);
  LOG_DBG("ac5 = %d", (int)data->calibration.coefficients.ac5);
  LOG_DBG("ac6 = %d", (int)data->calibration.coefficients.ac6);
  LOG_DBG(" b1 = %d", (int)data->calibration.coefficients.b1);
  LOG_DBG(" b2 = %d", (int)data->calibration.coefficients.b2);
  LOG_DBG(" mb = %d", (int)data->calibration.coefficients.mb);
  LOG_DBG(" mc = %d", (int)data->calibration.coefficients.mc);
  LOG_DBG(" md = %d", (int)data->calibration.coefficients.md);

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
