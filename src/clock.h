#ifndef IMUBAR_CLOCK_H__
#define IMUBAR_CLOCK_H__

#include <zephyr/drivers/rtc.h>

#include <string>
#include <tuple>

#include "types.h"

static constexpr auto second = clock_scaler;

extern void initialize_clock();
extern bool set_rtc(rtc_time& rtctime);
extern void set_clock(Time const time);
extern void set_clock(rtc_time& rtctime);
extern bool set_clock_from_rtc(bool exact = false);
extern void adjust_clock(Time const time);
extern void adjust_clock(rtc_time& rtctime);
extern void adjust_clock_from_rtc();
extern Time get_time();
extern std::tuple<Time, Time> get_time_and_uptime();
extern std::string get_time_str(bool include_date = true);
extern time_t rtc_time_to_time(rtc_time rtctime, bool include_millis = true);

#endif
