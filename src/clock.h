#ifndef IMUBAR_CLOCK_H__
#define IMUBAR_CLOCK_H__

#include <zephyr/drivers/rtc.h>

#include <string>
#include <tuple>

#include "types.h"

namespace imubar {
namespace clock {

extern void initialize();
extern bool set_rtc(rtc_time& rtctime, bool set_clock = true);
extern void set(Time const time);
extern void set(rtc_time& rtctime);
extern bool set_from_rtc(bool exact = false);
extern void adjust(Time const time);
extern void adjust(rtc_time& rtctime);
extern void adjust_from_rtc();
extern Time get_time();
extern std::tuple<Time, Time> get_time_and_uptime();
extern std::string get_time_str(bool include_date = true);
extern std::string get_date_str();
extern Time rtc_time_to_time(rtc_time rtctime, bool include_millis = true);
extern int32_t get_adjustment();

} // namespace clock


inline constexpr Time operator""_s(unsigned long long int literal) {
  return literal * clock_scaler;
}
inline constexpr Time operator""_ms(unsigned long long int literal) {
  return literal;
}
inline constexpr int operator""_rtcms(unsigned long long int literal) {
  return literal * 1'000'000;
}

} // namespace imubar

#endif
