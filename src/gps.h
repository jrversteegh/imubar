#ifndef IMUBAR_GPS_H__
#define IMUBAR_GPS_H__

#include "types.h"

namespace imubar {

namespace gnss {

extern void initialize();
extern bool has_fix();
extern bool has_data();
extern Position get_position();
extern Velocity get_velocity();

} // namespace gnss
} // namespace imubar

#endif
