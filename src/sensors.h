#ifndef IMUBAR_SENSORS_H__
#define IMUBAR_SENSORS_H__

#include "functions.h"
#include "geometry.h"

struct Vector3 {
  Number x;
  Number y;
  Number z;
};

void initialize_sensors();
Vector3 get_acceleration();
Vector3 get_rotation();
Vector3 get_magfield();

#endif
