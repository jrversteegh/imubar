#ifndef IMUBAR_TYPES_H__
#define IMUBAR_TYPES_H__

#include <cstdint>
#include <time.h>

using Number = float;
using Time = time_t;

struct Vector3 {
  Number x;
  Number y;
  Number z;
};

struct Sample {
  Time time;
};

struct ImuSample : Sample {
  Vector3 accel;
  Vector3 gyro;
  Vector3 magn;
};

struct EnvSample : Sample {
  Number temp;
  Number press;
};

struct Position {
  Number latitude;
  Number longitude;
};

struct Velocity {
  Number speed;
  Number bearing;
};

#endif
