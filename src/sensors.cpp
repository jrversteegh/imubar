#include <memory>
#include <string>
#include <type_traits>

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(imubar);

#include "errors.h"
#include "geometry.h"
#include "sensors.h"


#define MPU9250_0 DT_NODELABEL(mpu9250_0)
#define FXOS8700_0 DT_NODELABEL(fxos8700_0)
#define FXAS21002_0 DT_NODELABEL(fxas21002_0)
#define LSM303ACCEL_0 DT_NODELABEL(lsm303accel_0)
#define LSM303MAGN_0 DT_NODELABEL(lsm303magn_0)
#define BNO055_0 DT_NODELABEL(bno055_0)

static device const *const imu_mpu9250 = DEVICE_DT_GET(MPU9250_0);
static device const *const imu_fxos8700 = DEVICE_DT_GET(FXOS8700_0);
static device const *const imu_fxas21002 = DEVICE_DT_GET(FXAS21002_0);
static device const *const imu_lsm303accel = DEVICE_DT_GET(LSM303ACCEL_0);
static device const *const imu_lsm303magn = DEVICE_DT_GET(LSM303MAGN_0);
static device const *const imu_bno055 = DEVICE_DT_GET(BNO055_0);

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

std::vector<device const *> get_sensors() {
  return {
      imu_mpu9250,     imu_fxos8700,   imu_fxas21002,
      imu_lsm303accel, imu_lsm303magn, imu_bno055,
  };
}

void fetch_sensor(device const *sensor, sensor_channel channel) {
  if (sensor == nullptr)
    return;
  auto ret = sensor_sample_fetch(sensor);
  if (ret < 0 && ret != -EBADMSG) {
    error(2, "Sensor fetch error: %d", ret);
  }
}

Vector3 read_sensor(device const *sensor, sensor_channel channel) {
  if (sensor == nullptr)
    return Vector3{};
  struct sensor_value values[3];
  Vector3 result{};

  auto ret = sensor_channel_get(sensor, channel, values);
  if (ret < 0) {
    error(2, "Cannot read sensor: %d", ret);
    return result;
  }

  result.x = sensor_value_to<decltype(result.x)>(values[0]);
  result.y = sensor_value_to<decltype(result.y)>(values[1]);
  result.z = sensor_value_to<decltype(result.z)>(values[2]);
  return result;
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
  if (!device_is_ready(imu_bno055)) {
    error(2, "BNO055 not ready.");
  }
}

std::vector<std::unique_ptr<Imu>> &get_imus() {
  static std::vector<std::unique_ptr<Imu>> imus{};
  if (imus.size() == 0) {
    imus.push_back(std::make_unique<Imu>("adafruit_nxp_fx", imu_fxos8700,
                                         imu_fxas21002, imu_fxos8700));
    imus.push_back(std::make_unique<Imu>("sparkfun_mpu9250", imu_mpu9250,
                                         imu_mpu9250, imu_mpu9250, 12));
    imus.push_back(std::make_unique<Imu>("adafruit_lsm_l3", imu_lsm303accel,
                                         nullptr, imu_lsm303magn, 10));
    imus.push_back(std::make_unique<Imu>("adafruit_bno055", imu_bno055,
                                         imu_bno055, imu_bno055));
  }
  return imus;
}
