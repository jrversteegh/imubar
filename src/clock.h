#ifndef IMUBAR_CLOCK_H__
#define IMUBAR_CLOCK_H__

#include <zephyr/drivers/rtc.h>

#include <string>
#include <tuple>

#include "types.h"

extern void initialize_clock();
extern bool set_rtc(rtc_time& rtctime, bool so_set_clock = true);
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
extern int32_t get_clock_adjustment();

inline constexpr Time operator""_s(unsigned long long int literal) {
  return literal * clock_scaler;
}
inline constexpr Time operator""_ms(unsigned long long int literal) {
  return literal;
}
inline constexpr int operator""_rtcms(unsigned long long int literal) {
  return (literal % 1000) * 1000000;
}

#endif
