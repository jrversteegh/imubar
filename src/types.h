#ifndef IMUBAR_TYPES_H__
#define IMUBAR_TYPES_H__

#include <cstdint>

namespace imubar {

using Number = float;
using Time = int64_t;

// Represent time in milliseconds
static constexpr Time const clock_scaler = 1000;

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
  Number lat() const {
    return latitude;
  }
  Number lon() const {
    return longitude;
  }
};

struct Velocity {
  Number speed;
  Number bearing;
};

} // namespace imubar

#endif
