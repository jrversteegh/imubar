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
  int count = 0;
  for (int i = 0; i < 1000; ++i) {
    auto state = sensor_sample_fetch_chan(env_bmp180, SENSOR_CHAN_PRESS);
    //LOG_INF("Fetch state: %d", state);
    if (state < 0) {
      LOG_ERR("Error fetching sample");
      return -EIO;
    }
    if (state == 0) {
      ++count;
      struct sensor_value value;
      sensor_channel_get(env_bmp180, SENSOR_CHAN_AMBIENT_TEMP, &value);
      LOG_INF("Temp: %d", value.val1);

      sensor_channel_get(env_bmp180, SENSOR_CHAN_PRESS, &value);
      LOG_INF("Pressure: %d", value.val1);
    }
  }
  LOG_INF("BMP180 test done: %d reads.", count);
  return 0;
}
