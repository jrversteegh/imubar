/* lsm9ds1.h - header file for LSM9DS1 accelerometer and gyroscope sensor driver */

/*
 * Copyright (c) 2024 J.R. Versteegh
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_DRIVERS_SENSOR_LSM9DS1_LSMDS1_H_
#define ZEPHYR_DRIVERS_SENSOR_LSM9DS1_LSMDS1_H_

#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/util.h>
#include <zephyr/types.h>

#define DEG2RAD 0.017453292519943295769236907684

#define LSM9DS1_REG_WHO_AM_I 0x0F
#define LSM9DS1_VAL_WHO_AM_I 0x68

#define LSM9DS1_REG_INT1_CTRL 0x0C
#define LSM9DS1_REG_INT2_CTRL 0x0D

#define LSM9DS1_REG_CTRL_REG1 0x10
#define LSM9DS1_REG_CTRL_REG2 0x11
#define LSM9DS1_REG_CTRL_REG3 0x12
#define LSM9DS1_REG_ORIENT_CFG 0x13
#define LSM9DS1_REG_INT_GEN_SRC 0x14

#define LSM9DS1_REG_OUT_TEMP_L 0x15
#define LSM9DS1_REG_OUT_TEMP_H 0x16

#define LSM9DS1_REG_STATUS_REG 0x17

#define LSM9DS1_REG_OUT_X_L 0x18
#define LSM9DS1_REG_OUT_X_H 0x19
#define LSM9DS1_REG_OUT_Y_L 0x1A
#define LSM9DS1_REG_OUT_Y_H 0x1B
#define LSM9DS1_REG_OUT_Z_L 0x1C
#define LSM9DS1_REG_OUT_Z_H 0x1D

#define LSM9DS1_REG_CTRL_REG4 0x1E
#define LSM9DS1_REG_CTRL_REG5 0x1F
#define LSM9DS1_REG_CTRL_REG6 0x20
#define LSM9DS1_REG_CTRL_REG7 0x21
#define LSM9DS1_REG_CTRL_REG8 0x22
#define LSM9DS1_REG_CTRL_REG9 0x23
#define LSM9DS1_REG_CTRL_REG10 0x24

#define LSM9DS1_REG_INT_GEN_SRC_XL 0x26
#define LSM9DS1_REG_STATUS_REG_XL 0x27

#define LSM9DS1_REG_OUT_X_L_XL 0x28
#define LSM9DS1_REG_OUT_X_H_XL 0x29
#define LSM9DS1_REG_OUT_Y_L_XL 0x2A
#define LSM9DS1_REG_OUT_Y_H_XL 0x2B
#define LSM9DS1_REG_OUT_Z_L_XL 0x2C
#define LSM9DS1_REG_OUT_Z_H_XL 0x2D

#if defined(CONFIG_LSM9DS1_FULLSCALE_245)
#define LSM9DS1_DEFAULT_FULLSCALE 0
#elif defined(CONFIG_LSM9DS1_FULLSCALE_500)
#define LSM9DS1_DEFAULT_FULLSCALE 1
#elif defined(CONFIG_LSM9DS1_FULLSCALE_2000)
#define LSM9DS1_DEFAULT_FULLSCALE 2
#endif

#if defined(CONFIG_LSM9DS1_SAMPLING_RATE_95)
#define LSM9DS1_DEFAULT_SAMPLING_RATE 0
#elif defined(CONFIG_LSM9DS1_SAMPLING_RATE_190)
#define LSM9DS1_DEFAULT_SAMPLING_RATE 1
#elif defined(CONFIG_LSM9DS1_SAMPLING_RATE_380)
#define LSM9DS1_DEFAULT_SAMPLING_RATE 2
#elif defined(CONFIG_LSM9DS1_SAMPLING_RATE_760)
#define LSM9DS1_DEFAULT_SAMPLING_RATE 3
#endif


struct lsm9ds1_config {
  struct i2c_dt_spec i2c;

#if !defined(CONFIG_LSM9DS1_TRIGGER_NONE)
  struct gpio_dt_spec int_gpio;
#endif
};

struct lsm9ds1_data {
#if defined(CONFIG_LSM9DS1_TRIGGERS)
  struct k_sem sem;
#endif

#if !defined(CONFIG_LSM9DS1_TRIGGER_OWN_THREAD)
  K_KERNEL_STACK_MEMBER(thread_stack, CONFIG_LSM9DS1_THREAD_STACK_SIZE);
  struct k_thread thread;
  const struct device *dev;

  struct gpio_callback gpio_cb;
  const struct sensor_trigger *trigger_drdy;
  sensor_trigger_handler_t handler_drdy;
#endif

  int accel_x, accel_y, accel_z;
  int gyro_x, gyro_y, gyro_z;
#if defined(CONFIG_LSM9DS1_SEt_RUNTIME)
  uint8_t sample_fs;
  uint8_t fs;
#endif
};

#if !defined(CONFIG_LSM9DS1_TRIGGER_NONE)
int lsm9ds1_trigger_set(const struct device *dev,
                             const struct sensor_trigger *trig,
                             sensor_trigger_handler_t handler);

int lsm9ds1_init_interrupt(const struct device *dev);
#endif

#endif /* ZEPHYR_DRIVERS_SENSOR_lsm9ds1_LSMDS1_H_ */
