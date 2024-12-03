#include <memory>
#include <string>
#include <type_traits>

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "errors.h"
#include "sensors.h"
#include "types.h"

LOG_MODULE_DECLARE(imubar);

#define MPU9250_0 DT_NODELABEL(mpu9250_0)
#define FXOS8700_0 DT_NODELABEL(fxos8700_0)
#define FXAS21002_0 DT_NODELABEL(fxas21002_0)
#define LSM303ACCEL_0 DT_NODELABEL(lsm303accel_0)
#define LSM303MAGN_0 DT_NODELABEL(lsm303magn_0)
#define L3GD20H_0 DT_NODELABEL(l3gd20h_0)
#define BNO055_0 DT_NODELABEL(bno055_0)
#define LSM9DS1AG_0 DT_NODELABEL(lsm9ds1ag_0)
#define LSM9DS1MAGN_0 DT_NODELABEL(lsm9ds1magn_0)
#define BMP180_0 DT_NODELABEL(bmp180_0)

#define HMC5883L_0 DT_NODELABEL(hmc5883l_0)
#define BMA180_0 DT_NODELABEL(bma180_0)
#define ITG3205_0 DT_NODELABEL(itg3205_0)
#define BMP085_0 DT_NODELABEL(bmp085_0)
#define ICM20948_0 DT_NODELABEL(icm20948_0)
#define H3LIS331DL_0 DT_NODELABEL(h3lis331dl_0)

static device const *const imu_mpu9250 = DEVICE_DT_GET(MPU9250_0);
static device const *const imu_fxos8700 = DEVICE_DT_GET(FXOS8700_0);
static device const *const imu_fxas21002 = DEVICE_DT_GET(FXAS21002_0);
static device const *const imu_lsm303accel = DEVICE_DT_GET(LSM303ACCEL_0);
static device const *const imu_lsm303magn = DEVICE_DT_GET(LSM303MAGN_0);
static device const *const imu_l3gd20h = DEVICE_DT_GET(L3GD20H_0);
static device const *const imu_bno055 = DEVICE_DT_GET(BNO055_0);
static device const *const imu_lsm9ds1ag = DEVICE_DT_GET(LSM9DS1AG_0);
static device const *const imu_lsm9ds1magn = DEVICE_DT_GET(LSM9DS1MAGN_0);
static device const *const env_bmp180 = DEVICE_DT_GET(BMP180_0);

static device const *const imu_hmc5883l = DEVICE_DT_GET(HMC5883L_0);
static device const *const imu_bma180 = DEVICE_DT_GET(BMA180_0);
static device const *const imu_itg3205 = DEVICE_DT_GET(ITG3205_0);
static device const *const env_bmp085 = DEVICE_DT_GET(BMP085_0);
static device const *const imu_icm20948 = DEVICE_DT_GET(ICM20948_0);
static device const *const imu_h3lis331dl = DEVICE_DT_GET(H3LIS331DL_0);

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
      imu_mpu9250,    imu_fxos8700, imu_fxas21002, imu_lsm303accel, imu_l3gd20h,
      imu_lsm303magn, imu_bno055,   imu_lsm9ds1ag, imu_lsm9ds1magn,
  };
}

int fetch_sensor(device const *sensor, sensor_channel channel) {
  if (sensor == nullptr)
    return 0;
  auto ret = sensor_sample_fetch_chan(sensor, channel);
  if (ret < 0) {
    switch (ret) {
    case -EBADMSG:
      LOG_ERR("I2C communication error: %d", ret);
      break;
    default:
      LOG_ERR("Sensor fetch error: %d", ret);
    }
  }
  return ret;
}

Number read_sensor(device const *sensor, sensor_channel channel) {
  if (sensor == nullptr)
    return Number{};
  struct sensor_value value;
  Number result;
  auto ret = sensor_channel_get(sensor, channel, &value);
  if (ret < 0) {
    error(2, "Cannot read sensor: %d", ret);
    return result;
  }

  return sensor_value_to<decltype(result)>(value);
}

Vector3 read_sensor_vector(device const *sensor, sensor_channel channel) {
  if (sensor == nullptr)
    return Vector3{};
  struct sensor_value values[3];
  Vector3 result{};

  auto ret = sensor_channel_get(sensor, channel, values);
  if (ret < 0) {
    error(2, "Cannot read sensor vector: %d", ret);
    return result;
  }

  result.x = sensor_value_to<decltype(result.x)>(values[0]);
  result.y = sensor_value_to<decltype(result.y)>(values[1]);
  result.z = sensor_value_to<decltype(result.z)>(values[2]);
  return result;
}

void initialize_sensors() {
  // I2C Bus 0
  if (!device_is_ready(imu_mpu9250)) {
    error(2, "MPU9250 9dof not ready.");
  }
  if (!device_is_ready(imu_fxos8700)) {
    error(2, "FXOS8700 not ready.");
  }
  struct sensor_value odr = {.val1 = 100, .val2 = 0};
  if (sensor_attr_set(imu_fxos8700, SENSOR_CHAN_ALL,
                      SENSOR_ATTR_SAMPLING_FREQUENCY, &odr) != 0) {
    error(2, "Failed to set FXOS8700 odr.");
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
  if (!device_is_ready(imu_l3gd20h)) {
    error(2, "L3GD20H gyro not ready.");
  }
  if (!device_is_ready(imu_bno055)) {
    error(2, "BNO055 not ready.");
  }
  if (!device_is_ready(imu_lsm9ds1ag)) {
    error(2, "LSM9DS1 not ready.");
  }
  if (!device_is_ready(imu_lsm9ds1magn)) {
    error(2, "LSM9DS1 magn not ready.");
  }
  if (!device_is_ready(env_bmp180)) {
    error(2, "BMP180 pressure not ready.");
  }
  fetch_sensor(env_bmp180, SENSOR_CHAN_ALL);

  // I2C Bus 1
  if (!device_is_ready(imu_hmc5883l)) {
    error(2, "HMC5883L magn not ready.");
  }
  if (!device_is_ready(imu_bma180)) {
    error(2, "BMA180 accel not ready.");
  }
  if (!device_is_ready(imu_itg3205)) {
    error(2, "ITG3205 gyro not ready.");
  }
  if (!device_is_ready(env_bmp085)) {
    error(2, "BMP085 pressure not ready.");
  }
  fetch_sensor(env_bmp085, SENSOR_CHAN_ALL);
  if (!device_is_ready(imu_icm20948)) {
    error(2, "ICM20948 9dof not ready.");
  }
  if (!device_is_ready(imu_h3lis331dl)) {
    error(2, "H3LIS331DL high g accel not ready.");
  }
}

std::vector<std::unique_ptr<Imu>> &get_imus_bus0() {
  static std::vector<std::unique_ptr<Imu>> imus{};
  if (imus.size() == 0) {
    imus.push_back(std::make_unique<Imu>("adafruit_nxp_fx", imu_fxos8700,
                                         imu_fxas21002, imu_fxos8700));
    imus.push_back(std::make_unique<Imu>("sparkfun_mpu9250", imu_mpu9250,
                                         imu_mpu9250, imu_mpu9250, 12));
    imus.push_back(std::make_unique<Imu>("ada_lsm303_l3gd20", imu_lsm303accel,
                                         imu_l3gd20h, imu_lsm303magn, 25));
    imus.push_back(std::make_unique<Imu>("sparkfun_lsm9ds1", imu_lsm9ds1ag,
                                         imu_lsm9ds1ag, imu_lsm9ds1magn, 10));
  }
  return imus;
}

std::vector<std::unique_ptr<Imu>> &get_imus_bus1() {
  static std::vector<std::unique_ptr<Imu>> imus{};
  if (imus.size() == 0) {
    imus.push_back(std::make_unique<Imu>("nameless_10dof", imu_bma180,
                                         imu_itg3205, imu_hmc5883l, 7));
    imus.push_back(std::make_unique<Imu>("adafruit_bno055", imu_bno055,
                                         imu_bno055, imu_bno055));
    imus.push_back(std::make_unique<Imu>("pimoroni_icm20948", imu_icm20948,
                                         imu_icm20948, imu_icm20948));
    imus.push_back(std::make_unique<Imu>("sparkfun_h3lis331dl", imu_h3lis331dl,
                                         nullptr, nullptr));
  }
  return imus;
}

std::vector<std::unique_ptr<Env>> &get_envs_bus0() {
  static std::vector<std::unique_ptr<Env>> envs{};
  if (envs.size() == 0) {
    envs.push_back(std::make_unique<Env>("adafruit_bmp180", env_bmp180));
  }
  return envs;
}

std::vector<std::unique_ptr<Env>> &get_envs_bus1() {
  static std::vector<std::unique_ptr<Env>> envs{};
  if (envs.size() == 0) {
    envs.push_back(std::make_unique<Env>("nameless_bmp085", env_bmp085));
  }
  return envs;
}
