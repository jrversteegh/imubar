#ifndef IMUBAR_FUNCTIONS_H__
#define IMUBAR_FUNCTIONS_H__

#include <cmath>
#include <numbers>

using Number = float;

static constexpr Number pi = std::numbers::pi_v<Number>;

inline Number angle_mod(Number const angle) {
  Number result = std::fmod(angle, Number{360.0});
  return result < Number{0.0} ? result + Number{360.0} : result;
}

inline Number angle_mod_signed(Number const angle) {
  Number result = std::fmod(angle, Number{360.0});
  return result < Number{-180.0}   ? result + Number{360.0}
         : result >= Number{180.0} ? result - Number{360.0}
                                   : result;
}

inline Number angle_diff(Number const angle1, Number const angle2) {
  return angle_mod_signed(angle1 - angle2);
}

#endif
