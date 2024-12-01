#ifndef IMUBAR_TYPES_H__
#define IMUBAR_TYPES_H__

#include <cstdint>

using Number = float;
using Time = int64_t;

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

#endif
