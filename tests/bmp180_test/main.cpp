/*
 * Copyright (c) 2024 J.R. Versteegh
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/util.h>

#define BMP180_0 DT_NODELABEL(bmp180_0)

LOG_MODULE_REGISTER(bmp180_test);

int main(void) {
  LOG_INF("Starting BMP180 test...");
  static device const *const env_bmp180 = DEVICE_DT_GET(BMP180_0);
  if (!device_is_ready(env_bmp180)) {
    LOG_ERR("BMP180 pressure not ready.");
    return -1;
  }
  for (int i = 0; i < 120; ++i) {
    auto ret = sensor_sample_fetch(env_bmp180);
    if (ret < 0) {
      LOG_ERR("Error fetching sample");
      return -EIO;
    }
    struct sensor_value value;
    ret = sensor_channel_get(env_bmp180, SENSOR_CHAN_AMBIENT_TEMP, &value);
    LOG_INF("Temp: %d", value.val1);

    ret = sensor_channel_get(env_bmp180, SENSOR_CHAN_PRESS, &value);
    LOG_INF("Pressure: %d", value.val1);

    k_sleep(K_MSEC(1000));
  }
  LOG_INF("BMP180 test done.");
  return 0;
}
