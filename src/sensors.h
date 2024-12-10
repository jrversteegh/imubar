#ifndef IMUBAR_SENSORS_H__
#define IMUBAR_SENSORS_H__

#include <memory>
#include <string>
#include <vector>

#include <zephyr/device.h>

#include "clock.h"
#include "functions.h"
#include "types.h"

extern void initialize_sensors();
extern std::vector<device const *> get_sensors();
extern int fetch_sensor(device const *sensor,
                        sensor_channel channel = SENSOR_CHAN_ALL);
extern Number read_sensor(device const *sensor, sensor_channel channel);
extern Vector3 read_sensor_vector(device const *sensor, sensor_channel channel);

struct Sensor {
  Sensor(std::string const name) : name_(name) {}

  int fetch_sensor_with_error_count(device const *sensor,
                                    sensor_channel channel) {
    int ret = 0;
    if ((ret = fetch_sensor(sensor, channel)) < 0) {
      ++error_counter_;
      if (error_counter_ > 10) {
        error(2, "Permanent failure fetching from device: %s, code %d",
              name_.c_str(), ret);
      }
    } else {
      error_counter_ = 0;
    }
    return ret;
  }

  std::string get_name() { return name_; }

private:
  std::string name_ = "";
  int error_counter_ = 0;
};

struct Imu : Sensor {
  Imu(std::string const name, device const *const accel_device,
      device const *const gyro_device, device const *const magn_device,
      int magn_rate_divisor = 1)
      : Sensor(name), accel_device_(accel_device), gyro_device_(gyro_device),
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

  Time fetch() {
    fetch_sensor_with_error_count(accel_device_, accel_channel_);
    if (fetch_gyro_) {
      fetch_sensor_with_error_count(gyro_device_, gyro_channel_);
    }
    if (fetch_magn_ && (fetch_counter_ % magn_rate_divisor_ == 0)) {
      fetch_sensor_with_error_count(magn_device_, magn_channel_);
    }
    time_ = ::get_time();
    ++fetch_counter_;
    return time_;
  }

  Vector3 get_acceleration() {
    return read_sensor_vector(accel_device_, SENSOR_CHAN_ACCEL_XYZ);
  }

  Vector3 get_rotation() {
    return read_sensor_vector(gyro_device_, SENSOR_CHAN_GYRO_XYZ);
  }

  Vector3 get_magfield() {
    return read_sensor_vector(magn_device_, SENSOR_CHAN_MAGN_XYZ);
  }

  Time get_time() { return time_; }

private:
  device const *const accel_device_ = nullptr;
  device const *const gyro_device_ = nullptr;
  device const *const magn_device_ = nullptr;
  int magn_rate_divisor_ = 1;
  bool fetch_all = false;
  bool fetch_gyro_ = false;
  bool fetch_magn_ = false;
  Time time_ = 0;
  sensor_channel accel_channel_ = SENSOR_CHAN_ALL;
  sensor_channel gyro_channel_ = SENSOR_CHAN_ALL;
  sensor_channel magn_channel_ = SENSOR_CHAN_ALL;
  int fetch_counter_ = 0;
  int error_counter_ = 0;
};

struct Env : Sensor {
  Env(std::string name, device const *const press_device)
      : Sensor(name), press_device_(press_device) {}

  Time fetch() {
    fetch_sensor_with_error_count(press_device_, press_channel_);
    time_ = ::get_time();
    ++fetch_counter_;
    return time_;
  }

  Number get_temperature() {
    return read_sensor(press_device_, SENSOR_CHAN_AMBIENT_TEMP);
  }

  Number get_pressure() {
    return read_sensor(press_device_, SENSOR_CHAN_PRESS);
  }

  Time get_time() { return time_; }

private:
  device const *const press_device_ = nullptr;
  Time time_ = 0;
  sensor_channel press_channel_ = SENSOR_CHAN_PRESS;
  int fetch_counter_ = 0;
};

extern std::vector<std::unique_ptr<Imu>> &get_imus_bus0();
extern std::vector<std::unique_ptr<Imu>> &get_imus_bus1();
extern std::vector<std::unique_ptr<Env>> &get_envs_bus0();
extern std::vector<std::unique_ptr<Env>> &get_envs_bus1();

#endif
