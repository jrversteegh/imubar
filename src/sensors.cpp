#include <type_traits>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>

#include "errors.h"
#include "sensors.h"
#include "geometry.h"

static device const* const accelerometer = DEVICE_DT_GET(DT_ALIAS(accel0));
// Compass is useless because of the nearby motors, but initialize anyway
static device const* const magnetometer = DEVICE_DT_GET(DT_ALIAS(magn0));


struct None {};

template <typename T>
inline T sensor_value_to(sensor_value const& value) {
  static_assert(std::is_same_v<T, None>, "Conversion not supported");
  return 0;
}

template <>
inline float sensor_value_to<float>(sensor_value const& value) {
  return sensor_value_to_float(&value);
}

template <>
inline double sensor_value_to<double>(sensor_value const& value) {
  return sensor_value_to_double(&value);
}

static Vector3 read_sensor(device const* sensor, sensor_channel channel) {
  struct sensor_value values[3];
  Vector3 result{};

  auto ret = sensor_sample_fetch(sensor);
  if (ret < 0 && ret != -EBADMSG) {
    error(2, "Sensor sample update error.");
    return result;
  }

  ret = sensor_channel_get(sensor, channel, values);
  if (ret < 0) {
    error(2, "Cannot read sensor channels.");
    return result;
  }

  result.x = -sensor_value_to<decltype(result.x)>(values[2]);
  result.y = sensor_value_to<decltype(result.y)>(values[0]);
  result.z = sensor_value_to<decltype(result.z)>(values[1]);
  return result;
}

Vector3 get_acceleration() {
    return read_sensor(accelerometer, SENSOR_CHAN_ACCEL_XYZ);
}

Vector3 get_magfield() {
    return read_sensor(magnetometer, SENSOR_CHAN_MAGN_XYZ);
}

void initialize_sensors() {
  // Upon power on this will happen (not sure why). Will be fine after MCU reset
  if (!device_is_ready(accelerometer)) {
    printk("Accelerometer not ready.\n");
    sys_reboot(SYS_REBOOT_COLD);
    return;
  }

  if (!device_is_ready(magnetometer)) {
    error(2, "Magnetometer not ready.");
    return;
  }
}
