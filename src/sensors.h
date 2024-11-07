#ifndef IMUBAR_SENSORS_H__
#define IMUBAR_SENSORS_H__

#include <memory>
#include <string>
#include <vector>

#include <zephyr/device.h>

#include "functions.h"
#include "geometry.h"

struct Vector3 {
  Number x;
  Number y;
  Number z;
};

extern void initialize_sensors();
extern std::vector<device const *> get_sensors();
extern void fetch_sensor(device const *sensor,
                         sensor_channel channel = SENSOR_CHAN_ALL);
extern Vector3 read_sensor(device const *sensor, sensor_channel channel);

struct Imu {
  Imu(std::string name, device const *const accel_device,
      device const *const gyro_device, device const *const magn_device,
      int magn_rate_divisor = 1)
      : name_(name), accel_device_(accel_device), gyro_device_(gyro_device),
        magn_device_(magn_device), magn_rate_divisor_(magn_rate_divisor),
        fetch_gyro_(gyro_device != nullptr && gyro_device != accel_device),
        fetch_magn_(magn_device != nullptr && magn_device != accel_device &&
                    magn_device != gyro_device) {
    if (magn_rate_divisor != 1 && !fetch_magn_) {
      accel_channel_ = SENSOR_CHAN_ACCEL_XYZ;
      gyro_channel_ = SENSOR_CHAN_GYRO_XYZ;
      magn_channel_ = SENSOR_CHAN_MAGN_XYZ;
      fetch_gyro_ = true;
      fetch_magn_ = true;
    }
  }

  int64_t fetch() {
    fetch_sensor(accel_device_, accel_channel_);
    if (fetch_gyro_) {
      fetch_sensor(gyro_device_, gyro_channel_);
    }
    if (fetch_magn_ && (fetch_counter_ % magn_rate_divisor_ == 0)) {
      fetch_sensor(magn_device_, magn_channel_);
    }
    time_ = k_uptime_get();
    ++fetch_counter_;
    return time_;
  }

  std::string get_name() { return name_; }

  Vector3 get_acceleration() {
    return read_sensor(accel_device_, SENSOR_CHAN_ACCEL_XYZ);
  }

  Vector3 get_rotation() {
    return read_sensor(gyro_device_, SENSOR_CHAN_GYRO_XYZ);
  }

  Vector3 get_magfield() {
    return read_sensor(magn_device_, SENSOR_CHAN_MAGN_XYZ);
  }

  int64_t get_time() { return time_; }

private:
  std::string name_ = "";
  device const *const accel_device_ = nullptr;
  device const *const gyro_device_ = nullptr;
  device const *const magn_device_ = nullptr;
  int magn_rate_divisor_ = 1;
  bool fetch_gyro_ = false;
  bool fetch_magn_ = false;
  int64_t time_ = 0;
  sensor_channel accel_channel_ = SENSOR_CHAN_ALL;
  sensor_channel gyro_channel_ = SENSOR_CHAN_ALL;
  sensor_channel magn_channel_ = SENSOR_CHAN_ALL;
  int fetch_counter_ = 0;
};

extern std::vector<std::unique_ptr<Imu>> &get_imus();

#endif
