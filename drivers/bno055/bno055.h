/*
 * Copyright (c) 2024 J.R. Versteegh
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_DRIVERS_SENSOR_BNO055_BNO055_H_
#define ZEPHYR_DRIVERS_SENSOR_BNO055_BNO055_H_

#include <zephyr/device.h>
#include <zephyr/sys/util.h>
#include <zephyr/types.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

#define BNO055_REG_CHIP_ID		0x00
#if DT_INST_PROP(0, is_bmc150)
	#define BNO055_CHIP_ID		0xFA
#else
	#define BNO055_CHIP_ID		0xFB
#endif

#define BNO055_REG_PMU_BW		0x10
#if CONFIG_BNO055_PMU_BW_1
	#define BNO055_PMU_BW		0x08
#elif CONFIG_BNO055_PMU_BW_2
	#define BNO055_PMU_BW		0x09
#elif CONFIG_BNO055_PMU_BW_3
	#define BNO055_PMU_BW		0x0A
#elif CONFIG_BNO055_PMU_BW_4
	#define BNO055_PMU_BW		0x0B
#elif CONFIG_BNO055_PMU_BW_5
	#define BNO055_PMU_BW		0x0C
#elif CONFIG_BNO055_PMU_BW_6
	#define BNO055_PMU_BW		0x0D
#elif CONFIG_BNO055_PMU_BW_7
	#define BNO055_PMU_BW		0x0E
#elif CONFIG_BNO055_PMU_BW_8
	#define BNO055_PMU_BW		0x0F
#endif

/*
 * BNO055_PMU_FULL_RANGE measured in milli-m/s^2 instead
 * of m/s^2 to avoid using struct sensor_value for it
 */
#define BNO055_REG_PMU_RANGE		0x0F
#if CONFIG_BNO055_PMU_RANGE_2G
	#define BNO055_PMU_RANGE	0x03
	#define BNO055_PMU_FULL_RANGE	(4 * SENSOR_G)
#elif CONFIG_BNO055_PMU_RANGE_4G
	#define BNO055_PMU_RANGE	0x05
	#define BNO055_PMU_FULL_RANGE	(8 * SENSOR_G)
#elif CONFIG_BNO055_PMU_RANGE_8G
	#define BNO055_PMU_RANGE	0x08
	#define BNO055_PMU_FULL_RANGE	(16 * SENSOR_G)
#elif CONFIG_BNO055_PMU_RANGE_16G
	#define BNO055_PMU_RANGE	0x0C
	#define BNO055_PMU_FULL_RANGE	(32 * SENSOR_G)
#endif

#define BNO055_REG_TEMP			0x08

#define BNO055_REG_INT_STATUS_0		0x09
#define BNO055_BIT_SLOPE_INT_STATUS	BIT(2)
#define BNO055_REG_INT_STATUS_1		0x0A
#define BNO055_BIT_DATA_INT_STATUS	BIT(7)

#define BNO055_REG_INT_EN_0		0x16
#define BNO055_BIT_SLOPE_EN_X		BIT(0)
#define BNO055_BIT_SLOPE_EN_Y		BIT(1)
#define BNO055_BIT_SLOPE_EN_Z		BIT(2)
#define BNO055_SLOPE_EN_XYZ (BNO055_BIT_SLOPE_EN_X | \
		BNO055_BIT_SLOPE_EN_Y | BNO055_BIT_SLOPE_EN_X)

#define BNO055_REG_INT_EN_1		0x17
#define BNO055_BIT_DATA_EN		BIT(4)

#define BNO055_REG_INT_MAP_0		0x19
#define BNO055_INT_MAP_0_BIT_SLOPE	BIT(2)

#define BNO055_REG_INT_MAP_1		0x1A
#define BNO055_INT_MAP_1_BIT_DATA	BIT(0)

#define BNO055_REG_INT_RST_LATCH	0x21
#define BNO055_INT_MODE_LATCH		0x0F
#define BNO055_BIT_INT_LATCH_RESET	BIT(7)

#define BNO055_REG_INT_5		0x27
#define BNO055_SLOPE_DUR_SHIFT		0
#define BNO055_SLOPE_DUR_MASK		(3 << BNO055_SLOPE_DUR_SHIFT)

#define BNO055_REG_SLOPE_TH		0x28

#define BNO055_REG_ACCEL_X_LSB		0x2
#define BNO055_REG_ACCEL_Y_LSB		0x4
#define BNO055_REG_ACCEL_Z_LSB		0x6

#if DT_INST_PROP(0, is_bmc150)
	#define BNO055_ACCEL_LSB_BITS	4
	#define BNO055_ACCEL_LSB_SHIFT	4
#else
	#define BNO055_ACCEL_LSB_BITS	6
	#define BNO055_ACCEL_LSB_SHIFT	2
#endif
#define BNO055_ACCEL_LSB_MASK		\
		(BIT_MASK(BNO055_ACCEL_LSB_BITS) << BNO055_ACCEL_LSB_SHIFT)

#define BNO055_REG_ACCEL_X_MSB		0x3
#define BNO055_REG_ACCEL_Y_MSB		0x5
#define BNO055_REG_ACCEL_Z_MSB		0x7

#define BNO055_THREAD_PRIORITY		10
#define BNO055_THREAD_STACKSIZE_UNIT	1024

struct bno055_data {
	int16_t x_sample;
	int16_t y_sample;
	int16_t z_sample;
	int8_t temp_sample;

#ifdef CONFIG_BNO055_TRIGGER
	const struct device *dev;
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

int bno055_attr_set(const struct device *dev,
		    enum sensor_channel chan,
		    enum sensor_attribute attr,
		    const struct sensor_value *val);

int bno055_init_interrupt(const struct device *dev);
#endif

#endif /* ZEPHYR_DRIVERS_SENSOR_BNO055_BNO055_H_ */
