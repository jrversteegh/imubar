#ifndef IMUBAR_CLOCK_H__
#define IMUBAR_CLOCK_H__

#include <zephyr/drivers/rtc.h>

#include <string>

#include "types.h"

extern void initialize_clock();
extern bool set_rtc(rtc_time const &time);
extern void set_clock(Time const time);
extern Time get_time();
extern std::string get_time_str(bool include_date = true);

#endif
