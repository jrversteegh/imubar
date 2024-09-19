#ifndef IMUBAR_SENSORS_H__
#define IMUBAR_SENSORS_H__

#include <vector>

#include "functions.h"
#include "geometry.h"

struct Vector3 {
  Number x;
  Number y;
  Number z;
};

extern void initialize_sensors();
extern std::vector<device const *> get_sensors();
extern Vector3 read_sensor(device const *sensor, sensor_channel channel);


#endif
