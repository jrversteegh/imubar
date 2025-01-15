/*
 * Copyright (c) 2024 J.R. Versteegh
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_DRIVERS_SENSOR_BMP085_BMP085_H_
#define ZEPHYR_DRIVERS_SENSOR_BMP085_BMP085_H_

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>

#define BMP085_CHIP_ID_ADDR 0xD0
#define BMP085_CHIP_ID 0x55

// Defines taken from Adafruit library
#define BMP085_ULTRALOWPOWER 0 //!< Ultra low power mode
#define BMP085_STANDARD 1      //!< Standard mode
#define BMP085_HIGHRES 2       //!< High-res mode
#define BMP085_ULTRAHIGHRES 3  //!< Ultra high-res mode

#if defined(CONFIG_BMP085_OVERSAMPLING_LOW)
#define BMP085_OVERSAMPLING BMP085_ULTRALOWPOWER
#define BMP085_PRESSURE_READ_DELAY 5
#elif defined(CONFIG_BMP085_OVERSAMPLING_STANDARD)
#define BMP085_OVERSAMPLING BMP085_STANDARD
#define BMP085_PRESSURE_READ_DELAY 8
#elif defined(CONFIG_BMP085_OVERSAMPLING_HIGH)
#define BMP085_OVERSAMPLING BMP085_HIGHRES
#define BMP085_PRESSURE_READ_DELAY 14
#elif defined(CONFIG_BMP085_OVERSAMPLING_ULTRA)
#define BMP085_OVERSAMPLING BMP085_ULTRAHIGHRES
#define BMP085_PRESSURE_READ_DELAY 26
#else
#error "No oversampling selected"
#endif

#define NUM_CAL_COEFFS 11
#define BMP085_CAL_BASE 0xAA

#define BMP085_CAL_AC1 0xAA
#define BMP085_CAL_AC2 0xAC
#define BMP085_CAL_AC3 0xAE
#define BMP085_CAL_AC4 0xB0
#define BMP085_CAL_AC5 0xB2
#define BMP085_CAL_AC6 0xB4
#define BMP085_CAL_B1 0xB6
#define BMP085_CAL_B2 0xB8
#define BMP085_CAL_MB 0xBA
#define BMP085_CAL_MC 0xBC
#define BMP085_CAL_MD 0xBE

#define BMP085_CONTROL 0xF4
#define BMP085_TEMPDATA 0xF6
#define BMP085_PRESSUREDATA 0xF6
#define BMP085_READTEMPCMD 0x2E
#define BMP085_READPRESSURECMD 0x34

struct bmp085_coefficients {
  int16_t ac1;
  int16_t ac2;
  int16_t ac3;
  uint16_t ac4;
  uint16_t ac5;
  uint16_t ac6;
  int16_t b1;
  int16_t b2;
  int16_t mb;
  int16_t mc;
  int16_t md;
};

union bmp085_calibration {
  struct bmp085_coefficients coefficients;
  uint16_t data[NUM_CAL_COEFFS];
};

enum bmp085_fetch_state {
  bmp085_fetch_new,
  bmp085_temperature_commanded,
  bmp085_temperature_fetched,
  bmp085_pressure_commanded,
};

struct bmp085_data {
  uint16_t temperature;
  uint32_t pressure;
  const struct device* dev;
  union bmp085_calibration calibration;
  enum bmp085_fetch_state fetch_state;
};

struct bmp085_config {
  struct i2c_dt_spec i2c;
};

#endif /* ZEPHYR_DRIVERS_SENSOR_BMP085_BMP085_h_ */
