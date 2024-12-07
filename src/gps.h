#ifndef IMUBAR_GPS_H__
#define IMUBAR_GPS_H__

#include "types.h"

extern void initialize_gnss();

extern bool has_fix();
extern Position get_position();
extern Velocity get_velocity();

#endif
