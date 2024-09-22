#ifndef IMUBAR_SENSORS_H__
#define IMUBAR_SENSORS_H__

#include <memory>
#include <string>
#include <vector>

extern "C" {

#include <zephyr/device.h>
}

#include "functions.h"
#include "geometry.h"

struct Vector3 {
  Number x;
  Number y;
  Number z;
};

extern void initialize_sensors();
extern std::vector<device const *> get_sensors();
extern void fetch_sensor(device const *sensor, sensor_channel channel = SENSOR_CHAN_ALL);
extern Vector3 read_sensor(device const *sensor, sensor_channel channel);

struct Imu {
  Imu(std::string name,
      device const* const accel_device,
      device const* const gyro_device,
      device const* const magn_device):
    name_(name),
    accel_device_(accel_device),
    gyro_device_(gyro_device),
    magn_device_(magn_device) {
  }

  int64_t fetch() {
    fetch_sensor(accel_device_);
    if (gyro_device_ != accel_device_) {
      fetch_sensor(gyro_device_);
    }
    if (magn_device_ != accel_device_ && magn_device_ != gyro_device_) {
      fetch_sensor(magn_device_);
    }
    time_ = k_uptime_get();
    return time_;
  }

  std::string get_name() {
    return name_;
  }

  Vector3 get_acceleration() {
    return read_sensor(accel_device_, SENSOR_CHAN_ACCEL_XYZ);
  }

  Vector3 get_rotation() {
    return read_sensor(gyro_device_, SENSOR_CHAN_GYRO_XYZ);
  }

  Vector3 get_magfield() {
    return read_sensor(magn_device_, SENSOR_CHAN_MAGN_XYZ);
  }

  int64_t get_time() {
    return time_;
  }

private:
  std::string name_ = "";
  device const* const accel_device_ = nullptr;
  device const* const gyro_device_ = nullptr;
  device const* const magn_device_ = nullptr;
  int64_t time_ = 0;
};

extern std::vector<std::unique_ptr<Imu>>& get_imus();


#endif
