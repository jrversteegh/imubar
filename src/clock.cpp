#include <time.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/timeutil.h>

#include "clock.h"
#include "errors.h"

LOG_MODULE_DECLARE(imubar);

#define RTC_0 DT_NODELABEL(rtc_0)

static device const* const rtc = DEVICE_DT_GET(RTC_0);

static Time uptime_offset_ = 0;
static Time clock_set_at_ = 0;
static int32_t adjustment_ = 0;

inline time_t rtc_time_to_time(rtc_time rtctime, bool include_millis = false) {
  time_t time = timeutil_timegm(rtc_time_to_tm(&rtctime));
  auto result = clock_scaler * time;
  if (include_millis) {
    result += rtctime.tm_nsec / 1000000;
  }
  return result;
}

bool set_rtc(rtc_time& rtctime) {
  auto ret = rtc_set_time(rtc, &rtctime);
  if (ret < 0) {
    LOG_ERR("Failed to set RTC clock");
    return false;
  }
  set_clock(rtctime);
  return true;
}

void set_clock(Time time) {
  clock_set_at_ = k_uptime_get();
  uptime_offset_ = time - clock_set_at_;
}

void set_clock(rtc_time& rtctime) {
  set_clock(rtc_time_to_time(rtctime));
}

bool set_clock_from_rtc(bool exact) {
  rtc_time rtctime;
  auto ret = rtc_get_time(rtc, &rtctime);
  if (ret < 0) {
    LOG_ERR("Failed to get RTC time: %d", ret);
    return false;
  }
  if (exact) {
    auto secs = rtctime.tm_sec;
    while (rtctime.tm_sec == secs) {
      ret = rtc_get_time(rtc, &rtctime);
      if (ret < 0) {
        LOG_ERR("Failed to get RTC time: %d", ret);
        return false;
      }
      k_msleep(1);
    }
  }
  set_clock(rtctime);
  return true;
}

void adjust_clock(Time time) {
  auto offset = time - k_uptime_get();
  if (offset > uptime_offset_) {
    uptime_offset_ += 1;
    adjustment_ += 1;
  }
  else if (offset < uptime_offset_) {
    uptime_offset_ -= 1;
    adjustment_ -= 1;
  }
}

Time get_time() {
  return k_uptime_get() + uptime_offset_;
}
std::tuple<Time, Time> get_time_and_uptime() {
  auto uptime = k_uptime_get();
  return std::make_tuple(uptime, uptime + uptime_offset_);
}

std::string get_time_str(bool include_date) {
  static constexpr char const* const short_fmt = "%H:%M:%S";
  static constexpr char const* const long_fmt = "%m-%d %H:%M:%S";
  static constexpr int short_fmt_len = std::char_traits<char>::length(short_fmt);
  static constexpr int short_fmt_size = short_fmt_len + 1;
  static constexpr int long_fmt_len = std::char_traits<char>::length(long_fmt);
  static constexpr int long_fmt_size = long_fmt_len + 1;
  auto now = get_time() / clock_scaler;
  auto time = gmtime(&now);
  std::string result{};
  if (include_date) {
    result.resize(long_fmt_size);
    strftime(&result[0], long_fmt_size, long_fmt, time);
    result.resize(long_fmt_len);
  }
  else {
    result.resize(short_fmt_size);
    strftime(&result[0], short_fmt_size, short_fmt, time);
    result.resize(short_fmt_len);
  }
  return result;
}

void initialize_clock() {
  if (!device_is_ready(rtc)) {
    error(2, "RTC not ready.");
  }

  set_clock_from_rtc(true);
}
