/*
 * Copyright (c) 2024 J.R. Versteegh
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_DRIVERS_SENSOR_BNO055_BNO055_H_
#define ZEPHYR_DRIVERS_SENSOR_BNO055_BNO055_H_

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>

/* Defines taken from BNO055 linux driver: bno055.h -> */

/***************************************************/
/**\name    REGISTER ADDRESS DEFINITION  */
/***************************************************/
/* Page id register definition*/
#define BNO055_PAGE_ID_ADDR (0X07)

/* PAGE0 REGISTER DEFINITION START*/
#define BNO055_CHIP_ID_ADDR (0x00)
#define BNO055_ACCEL_REV_ID_ADDR (0x01)
#define BNO055_MAGN_REV_ID_ADDR (0x02)
#define BNO055_GYRO_REV_ID_ADDR (0x03)
#define BNO055_SW_REV_ID_LSB_ADDR (0x04)
#define BNO055_SW_REV_ID_MSB_ADDR (0x05)
#define BNO055_BL_REV_ID_ADDR (0X06)

/* Accel data register*/
#define BNO055_ACCEL_DATA_X_LSB_ADDR (0X08)
#define BNO055_ACCEL_DATA_X_MSB_ADDR (0X09)
#define BNO055_ACCEL_DATA_Y_LSB_ADDR (0X0A)
#define BNO055_ACCEL_DATA_Y_MSB_ADDR (0X0B)
#define BNO055_ACCEL_DATA_Z_LSB_ADDR (0X0C)
#define BNO055_ACCEL_DATA_Z_MSB_ADDR (0X0D)

/*Mag data register*/
#define BNO055_MAGN_DATA_X_LSB_ADDR (0X0E)
#define BNO055_MAGN_DATA_X_MSB_ADDR (0X0F)
#define BNO055_MAGN_DATA_Y_LSB_ADDR (0X10)
#define BNO055_MAGN_DATA_Y_MSB_ADDR (0X11)
#define BNO055_MAGN_DATA_Z_LSB_ADDR (0X12)
#define BNO055_MAGN_DATA_Z_MSB_ADDR (0X13)

/*Gyro data registers*/
#define BNO055_GYRO_DATA_X_LSB_ADDR (0X14)
#define BNO055_GYRO_DATA_X_MSB_ADDR (0X15)
#define BNO055_GYRO_DATA_Y_LSB_ADDR (0X16)
#define BNO055_GYRO_DATA_Y_MSB_ADDR (0X17)
#define BNO055_GYRO_DATA_Z_LSB_ADDR (0X18)
#define BNO055_GYRO_DATA_Z_MSB_ADDR (0X19)

/*Euler data registers*/
#define BNO055_EULER_H_LSB_ADDR (0X1A)
#define BNO055_EULER_H_MSB_ADDR (0X1B)

#define BNO055_EULER_R_LSB_ADDR (0X1C)
#define BNO055_EULER_R_MSB_ADDR (0X1D)

#define BNO055_EULER_P_LSB_ADDR (0X1E)
#define BNO055_EULER_P_MSB_ADDR (0X1F)

/*Quaternion data registers*/
#define BNO055_QUATERNION_DATA_W_LSB_ADDR (0X20)
#define BNO055_QUATERNION_DATA_W_MSB_ADDR (0X21)
#define BNO055_QUATERNION_DATA_X_LSB_ADDR (0X22)
#define BNO055_QUATERNION_DATA_X_MSB_ADDR (0X23)
#define BNO055_QUATERNION_DATA_Y_LSB_ADDR (0X24)
#define BNO055_QUATERNION_DATA_Y_MSB_ADDR (0X25)
#define BNO055_QUATERNION_DATA_Z_LSB_ADDR (0X26)
#define BNO055_QUATERNION_DATA_Z_MSB_ADDR (0X27)

/* Linear acceleration data registers*/
#define BNO055_LINEAR_ACCEL_DATA_X_LSB_ADDR (0X28)
#define BNO055_LINEAR_ACCEL_DATA_X_MSB_ADDR (0X29)
#define BNO055_LINEAR_ACCEL_DATA_Y_LSB_ADDR (0X2A)
#define BNO055_LINEAR_ACCEL_DATA_Y_MSB_ADDR (0X2B)
#define BNO055_LINEAR_ACCEL_DATA_Z_LSB_ADDR (0X2C)
#define BNO055_LINEAR_ACCEL_DATA_Z_MSB_ADDR (0X2D)

/*Gravity data registers*/
#define BNO055_GRAVITY_DATA_X_LSB_ADDR (0X2E)
#define BNO055_GRAVITY_DATA_X_MSB_ADDR (0X2F)
#define BNO055_GRAVITY_DATA_Y_LSB_ADDR (0X30)
#define BNO055_GRAVITY_DATA_Y_MSB_ADDR (0X31)
#define BNO055_GRAVITY_DATA_Z_LSB_ADDR (0X32)
#define BNO055_GRAVITY_DATA_Z_MSB_ADDR (0X33)

/* Temperature data register*/
#define BNO055_TEMP_ADDR (0X34)

/* Status registers*/
#define BNO055_CALIB_STAT_ADDR (0X35)
#define BNO055_SELFTEST_RESULT_ADDR (0X36)
#define BNO055_INTR_STAT_ADDR (0X37)
#define BNO055_SYS_CLK_STAT_ADDR (0X38)
#define BNO055_SYS_STAT_ADDR (0X39)
#define BNO055_SYS_ERR_ADDR (0X3A)

/* Unit selection register*/
#define BNO055_UNIT_SEL_ADDR (0X3B)
#define BNO055_DATA_SELECT_ADDR (0X3C)

/* Mode registers*/
#define BNO055_OPR_MODE_ADDR (0X3D)
#define BNO055_PWR_MODE_ADDR (0X3E)

#define BNO055_SYS_TRIGGER_ADDR (0X3F)
#define BNO055_TEMP_SOURCE_ADDR (0X40)

/* Axis remap registers*/
#define BNO055_AXIS_MAP_CONFIG_ADDR (0X41)
#define BNO055_AXIS_MAP_SIGN_ADDR (0X42)

/* SIC registers*/
#define BNO055_SIC_MATRIX_0_LSB_ADDR (0X43)
#define BNO055_SIC_MATRIX_0_MSB_ADDR (0X44)
#define BNO055_SIC_MATRIX_1_LSB_ADDR (0X45)
#define BNO055_SIC_MATRIX_1_MSB_ADDR (0X46)
#define BNO055_SIC_MATRIX_2_LSB_ADDR (0X47)
#define BNO055_SIC_MATRIX_2_MSB_ADDR (0X48)
#define BNO055_SIC_MATRIX_3_LSB_ADDR (0X49)
#define BNO055_SIC_MATRIX_3_MSB_ADDR (0X4A)
#define BNO055_SIC_MATRIX_4_LSB_ADDR (0X4B)
#define BNO055_SIC_MATRIX_4_MSB_ADDR (0X4C)
#define BNO055_SIC_MATRIX_5_LSB_ADDR (0X4D)
#define BNO055_SIC_MATRIX_5_MSB_ADDR (0X4E)
#define BNO055_SIC_MATRIX_6_LSB_ADDR (0X4F)
#define BNO055_SIC_MATRIX_6_MSB_ADDR (0X50)
#define BNO055_SIC_MATRIX_7_LSB_ADDR (0X51)
#define BNO055_SIC_MATRIX_7_MSB_ADDR (0X52)
#define BNO055_SIC_MATRIX_8_LSB_ADDR (0X53)
#define BNO055_SIC_MATRIX_8_MSB_ADDR (0X54)

/* Accelerometer Offset registers*/
#define BNO055_ACCEL_OFFSET_X_LSB_ADDR (0X55)
#define BNO055_ACCEL_OFFSET_X_MSB_ADDR (0X56)
#define BNO055_ACCEL_OFFSET_Y_LSB_ADDR (0X57)
#define BNO055_ACCEL_OFFSET_Y_MSB_ADDR (0X58)
#define BNO055_ACCEL_OFFSET_Z_LSB_ADDR (0X59)
#define BNO055_ACCEL_OFFSET_Z_MSB_ADDR (0X5A)

/* Magnetometer Offset registers*/
#define BNO055_MAGN_OFFSET_X_LSB_ADDR (0X5B)
#define BNO055_MAGN_OFFSET_X_MSB_ADDR (0X5C)
#define BNO055_MAGN_OFFSET_Y_LSB_ADDR (0X5D)
#define BNO055_MAGN_OFFSET_Y_MSB_ADDR (0X5E)
#define BNO055_MAGN_OFFSET_Z_LSB_ADDR (0X5F)
#define BNO055_MAGN_OFFSET_Z_MSB_ADDR (0X60)

/* Gyroscope Offset registers*/
#define BNO055_GYRO_OFFSET_X_LSB_ADDR (0X61)
#define BNO055_GYRO_OFFSET_X_MSB_ADDR (0X62)
#define BNO055_GYRO_OFFSET_Y_LSB_ADDR (0X63)
#define BNO055_GYRO_OFFSET_Y_MSB_ADDR (0X64)
#define BNO055_GYRO_OFFSET_Z_LSB_ADDR (0X65)
#define BNO055_GYRO_OFFSET_Z_MSB_ADDR (0X66)

/* Radius registers*/
#define BNO055_ACCEL_RADIUS_LSB_ADDR (0X67)
#define BNO055_ACCEL_RADIUS_MSB_ADDR (0X68)
#define BNO055_MAGN_RADIUS_LSB_ADDR (0X69)
#define BNO055_MAGN_RADIUS_MSB_ADDR (0X6A)

/* PAGE0 REGISTERS DEFINITION END*/
/* PAGE1 REGISTERS DEFINITION START*/
/* Configuration registers*/
#define BNO055_ACCEL_CONFIG_ADDR (0X08)
#define BNO055_MAGN_CONFIG_ADDR (0X09)
#define BNO055_GYRO_CONFIG_ADDR (0X0A)
#define BNO055_GYRO_MODE_CONFIG_ADDR (0X0B)
#define BNO055_ACCEL_SLEEP_CONFIG_ADDR (0X0C)
#define BNO055_GYRO_SLEEP_CONFIG_ADDR (0X0D)
#define BNO055_MAGN_SLEEP_CONFIG_ADDR (0x0E)

/* Interrupt registers*/
#define BNO055_INT_MASK_ADDR (0X0F)
#define BNO055_INT_ADDR (0X10)
#define BNO055_ACCEL_ANY_MOTION_THRES_ADDR (0X11)
#define BNO055_ACCEL_INTR_SETTINGS_ADDR (0X12)
#define BNO055_ACCEL_HIGH_G_DURN_ADDR (0X13)
#define BNO055_ACCEL_HIGH_G_THRES_ADDR (0X14)
#define BNO055_ACCEL_NO_MOTION_THRES_ADDR (0X15)
#define BNO055_ACCEL_NO_MOTION_SET_ADDR (0X16)
#define BNO055_GYRO_INTR_SETING_ADDR (0X17)
#define BNO055_GYRO_HIGHRATE_X_SET_ADDR (0X18)
#define BNO055_GYRO_DURN_X_ADDR (0X19)
#define BNO055_GYRO_HIGHRATE_Y_SET_ADDR (0X1A)
#define BNO055_GYRO_DURN_Y_ADDR (0X1B)
#define BNO055_GYRO_HIGHRATE_Z_SET_ADDR (0X1C)
#define BNO055_GYRO_DURN_Z_ADDR (0X1D)
#define BNO055_GYRO_ANY_MOTION_THRES_ADDR (0X1E)
#define BNO055_GYRO_ANY_MOTION_SET_ADDR (0X1F)

/* PAGE1 REGISTERS DEFINITION END*/

/* <- Defines taken from BNO055 linux driver: bno055.h */

#define BNO055_CHIP_ID 0xA0
#define BNO055_THREAD_PRIORITY 10
#define BNO055_THREAD_STACKSIZE_UNIT 1024

#define BNO055_ACCEL_LSB_PER_MS 100
#define BNO055_GYRO_LSB_PER_RPS 900
#define BNO055_MAGN_LSB_PER_GS 1600
#define BNO055_TEMP_LSB_PER_C 1

typedef union {
  uint16_t components[3];
  struct {
    uint16_t x;
    uint16_t y;
    uint16_t z;
  };
} bno055_vec;

typedef union {
  bno055_vec vectors[3];
  struct {
    bno055_vec accel;
    bno055_vec magn;
    bno055_vec gyro;
  };
} bno055_ninedof;

struct bno055_data {
  bno055_ninedof sample;
  uint8_t temp;

  const struct device *dev;
#ifdef CONFIG_BNO055_TRIGGER
  struct gpio_callback gpio_cb;

  const struct sensor_trigger *data_ready_trigger;
  sensor_trigger_handler_t data_ready_handler;

  const struct sensor_trigger *any_motion_trigger;
  sensor_trigger_handler_t any_motion_handler;

#if defined(CONFIG_BNO055_TRIGGER_OWN_THREAD)
  K_KERNEL_STACK_MEMBER(thread_stack, CONFIG_BNO055_THREAD_STACK_SIZE);
  struct k_thread thread;
  struct k_sem gpio_sem;
#elif defined(CONFIG_BNO055_TRIGGER_GLOBAL_THREAD)
  struct k_work work;
#endif

#endif /* CONFIG_BNO055_TRIGGER */
};

struct bno055_config {
  struct i2c_dt_spec i2c;
#ifdef CONFIG_BNO055_TRIGGER
  struct gpio_dt_spec int1_gpio;
#endif
};

#ifdef CONFIG_BNO055_TRIGGER
int bno055_trigger_set(const struct device *dev,
                       const struct sensor_trigger *trig,
                       sensor_trigger_handler_t handler);

int bno055_init_interrupt(const struct device *dev);
#endif

union opr_mode {
  uint8_t byte;
  struct {
    uint8_t unused : 4;
    uint8_t operation_mode : 4;
  } config;
};

enum opr_mode_operation_mode_values {
  operation_mode_config_mode = 0,
  // Non-fusion modes
  operation_mode_accel_only = 1,
  operation_mode_magn_only = 2,
  operation_mode_gyro_only = 3,
  operation_mode_accel_magn = 4,
  operation_mode_accel_gyro = 5,
  operation_mode_gyro_magn = 6,
  operation_mode_accel_gyro_magn = 7,
  // Fusion modes
  operation_mode_imu = 8,
  operation_mode_compass = 9,
  operation_mode_m4g = 10,
  operation_mode_ndof_fmc_off = 11,
  operation_mode_ndof = 12,
};

union pwr_mode {
  uint8_t byte;
  struct {
    uint8_t unused : 6;
    uint8_t power_mode : 2;
  } config;
};

enum pwr_mode_power_mode_values {
  power_mode_normal = 0,
  power_mode_low = 1,
  power_mode_suspend = 2
};

union axi_map_config {
  uint8_t byte;
  struct {
    uint8_t unused : 2;
    uint8_t z_axis : 2;
    uint8_t y_axis : 2;
    uint8_t x_axis : 2;
  } config;
};

enum axis_map_values {
  axis_map_x_axis = 0,
  axis_map_y_axis = 1,
  axis_map_z_axis = 2
};

union axi_map_sign {
  uint8_t byte;
  struct {
    uint8_t unused : 5;
    uint8_t z_axis_sign : 1;
    uint8_t y_axis_sign : 1;
    uint8_t x_axis_sign : 1;
  } config;
};

enum axis_map_sign_values { axis_sign_positive = 0, axis_sign_negative = 1 };

/* Accel config */

union accel_config {
  uint8_t byte;
  struct {
    uint8_t accel_power_mode : 3;
    uint8_t accel_bandwidth : 3;
    uint8_t accel_range : 2;

  } config;
};

enum accel_config_power_mode_values {
  accel_config_power_mode_normal = 0,
  accel_config_power_mode_suspend = 1,
  accel_config_power_mode_low_power1 = 2,
  accel_config_power_mode_standby = 3,
  accel_config_power_mode_low_power2 = 4
};

enum accel_config_bandwidth_values {
  accel_config_bandwidth_8Hz = 0,
  accel_config_bandwidth_16Hz = 1,
  accel_config_bandwidth_31Hz = 2,
  accel_config_bandwidth_62Hz = 3,
  accel_config_bandwidth_125Hz = 4,
  accel_config_bandwidth_250Hz = 5,
  accel_config_bandwidth_500Hz = 6,
  accel_config_bandwidth_1000Hz = 7
};

enum accel_config_range_values {
  accel_config_range_2g = 0,
  accel_config_range_4g = 1,
  accel_config_range_8g = 2,
  accel_config_range_16g = 3
};

/* Magn config */

union magn_config {
  uint8_t byte;
  struct {
    uint8_t unused : 1;
    uint8_t magn_power_mode : 2;
    uint8_t magn_operation_mode : 2;
    uint8_t magn_data_rate : 3;
  } config;
};

enum magn_config_power_mode_values {
  magn_config_power_mode_normal = 0,
  magn_config_power_mode_sleep = 1,
  magn_config_power_mode_suspend = 2,
  magn_config_power_mode_force = 2
};

enum magn_config_operation_mode_values {
  magn_config_operation_low_power = 0,
  magn_config_operation_regular = 1,
  magn_config_operation_enhanced = 2,
  magn_config_operation_high_accuracy = 3
};

enum magn_config_data_rate {
  magn_config_data_rate_2Hz = 0,
  magn_config_data_rate_6Hz = 1,
  magn_config_data_rate_8Hz = 2,
  magn_config_data_rate_10Hz = 3,
  magn_config_data_rate_15Hz = 4,
  magn_config_data_rate_20Hz = 5,
  magn_config_data_rate_25Hz = 6,
  magn_config_data_rate_30Hz = 7
};

/* Gyro config */

union gyro_config {
  uint8_t byte;
  struct {
    uint8_t unused : 2;
    uint8_t gyro_bandwidth : 3;
    uint8_t gyro_range : 3;
  } config;
};

enum gyro_config_bandwidth_values {
  gyro_mode_bandwidth_523Hz = 0,
  gyro_mode_bandwidth_230Hz = 1,
  gyro_mode_bandwidth_116Hz = 2,
  gyro_mode_bandwidth_47Hz = 3,
  gyro_mode_bandwidth_23Hz = 4,
  gyro_mode_bandwidth_12Hz = 5,
  gyro_mode_bandwidth_64Hz = 6,
  gyro_mode_bandwidth_32Hz = 7
};

enum gyro_config_range_values {
  gyro_mode_range_2000dps = 0,
  gyro_mode_range_1000dps = 1,
  gyro_mode_range_500dps = 2,
  gyro_mode_range_250dps = 3,
  gyro_mode_range_125dps = 4
};

union gyro_mode_config {
  uint8_t byte;
  struct {
    uint8_t unused : 5;
    uint8_t gyro_power_mode : 3;
  } config;
};

enum gyro_mode_config_operation_mode_values {
  gyro_mode_config_normal = 0,
  gyro_mode_config_fast_power_up = 1,
  gyro_mode_config_deep_suspend = 2,
  gyro_mode_config_suspend = 3,
  gyro_mode_config_advanced_powersave = 4
};

#endif /* ZEPHYR_DRIVERS_SENSOR_BNO055_BNO055_H_ */
