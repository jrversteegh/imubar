#include <type_traits>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>

#include "errors.h"
#include "geometry.h"
#include "sensors.h"

#define MPU9250_0 DT_NODELABEL(mpu9250_0)
#define FXOS8700_0 DT_NODELABEL(fxos8700_0)
#define FXAS21002_0 DT_NODELABEL(fxas21002_0)
#define LSM303ACCEL_0 DT_NODELABEL(lsm303accel_0)
#define LSM303MAGN_0 DT_NODELABEL(lsm303magn_0)

static device const *const imu_mpu9250 = DEVICE_DT_GET(MPU9250_0);
static device const *const imu_fxos8700 = DEVICE_DT_GET(FXOS8700_0);
static device const *const imu_fxas21002 = DEVICE_DT_GET(FXAS21002_0);
static device const *const imu_lsm303accel = DEVICE_DT_GET(LSM303ACCEL_0);
static device const *const imu_lsm303magn = DEVICE_DT_GET(LSM303MAGN_0);

struct None {};

template <typename T> inline T sensor_value_to(sensor_value const &value) {
  static_assert(std::is_same_v<T, None>, "Conversion not supported");
  return 0;
}

template <> inline float sensor_value_to<float>(sensor_value const &value) {
  return sensor_value_to_float(&value);
}

template <> inline double sensor_value_to<double>(sensor_value const &value) {
  return sensor_value_to_double(&value);
}

static Vector3 read_sensor(device const *sensor, sensor_channel channel) {
  struct sensor_value values[3];
  Vector3 result{};

  auto ret = sensor_sample_fetch(sensor);
  if (ret < 0 && ret != -EBADMSG) {
    error(2, "Sensor sample update error: %d", ret);
    return result;
  }

  ret = sensor_channel_get(sensor, channel, values);
  if (ret < 0) {
    error(2, "Cannot read sensor channels: %d", ret);
    return result;
  }

  result.x = -sensor_value_to<decltype(result.x)>(values[2]);
  result.y = sensor_value_to<decltype(result.y)>(values[0]);
  result.z = sensor_value_to<decltype(result.z)>(values[1]);
  return result;
}

Vector3 get_acceleration() {
  return read_sensor(imu_lsm303accel, SENSOR_CHAN_ACCEL_XYZ);
}

Vector3 get_magfield() {
  return read_sensor(imu_mpu9250, SENSOR_CHAN_MAGN_XYZ);
}

Vector3 get_rotation() {
  return read_sensor(imu_mpu9250, SENSOR_CHAN_GYRO_XYZ);
}

void initialize_sensors() {
  if (!device_is_ready(imu_mpu9250)) {
    error(2, "MPU9250 not ready.");
  }
  if (!device_is_ready(imu_fxos8700)) {
    error(2, "FXOS8700 not ready.");
  }
  if (!device_is_ready(imu_fxas21002)) {
    error(2, "FXAS21002 not ready.");
  }
  if (!device_is_ready(imu_lsm303accel)) {
    error(2, "LSM303 accel not ready.");
  }
  if (!device_is_ready(imu_lsm303magn)) {
    error(2, "LSM303 magn not ready.");
  }
}
