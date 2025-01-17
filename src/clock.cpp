#include <time.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/timeutil.h>

#include "clock.h"
#include "errors.h"
#include "gps.h"

LOG_MODULE_DECLARE(imubar);

#define RTC_0 DT_NODELABEL(rtc_0)

static device const* const rtc = DEVICE_DT_GET(RTC_0);

static Time uptime_offset_ = 0;
static Time clock_set_at_ = 0;
static int32_t adjustment_ = 0;

static rtc_time rtctime_to_set{};
void set_rtc_delayed(struct k_timer* dummy) {
  if (!set_rtc(rtctime_to_set)) {
    LOG_ERR("Failed to set RTC");
  }
}

K_TIMER_DEFINE(rtc_set_timer, set_rtc_delayed, NULL);

time_t rtc_time_to_time(rtc_time rtctime, bool include_millis) {
  time_t time = timeutil_timegm(rtc_time_to_tm(&rtctime));
  auto result = clock_scaler * time;
  if (include_millis) {
    result += rtctime.tm_nsec / 1000000;
  }
  return result;
}

bool set_rtc(rtc_time& rtctime) {
  if (rtctime.tm_nsec == 0) {
    auto ret = rtc_set_time(rtc, &rtctime);
    if (ret < 0) {
      LOG_ERR("Failed to set RTC clock");
      return false;
    }
  }
  else {
    rtctime_to_set = rtctime;
    rtctime_to_set.tm_sec += 1;
    rtctime_to_set.tm_nsec = 0;
    auto delay = 1000 - rtctime.tm_nsec / 1000000;
    k_timer_start(&rtc_set_timer, K_MSEC(delay), K_NO_WAIT);
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

void adjust_clock(rtc_time& rtctime) {
  adjust_clock(rtc_time_to_time(rtctime));
}

/**
 * Call multiple times a second to get accurate results. This is
 * because the RTC only has 1 second resolution and so we'll need
 * to poll the rtc often to get close to the second increment
 */
void adjust_clock_from_rtc() {
  // Only do any of this if there's no GPS fix.
  // When there is, the GPS will sync the clock
  if (!gnss::has_fix()) {
    rtc_time rtctime;
    auto ret = rtc_get_time(rtc, &rtctime);
    if (ret < 0) {
      LOG_ERR("Failed to get RTC time: %d", ret);
      return;
    }
    static Time last_time_from_rtc = 0;
    Time time_from_rtc = rtc_time_to_time(rtctime, false);
    // Do this on each minute mark only and when we ticked exactly 1 second
    if ((rtctime.tm_sec == 0) && ((time_from_rtc - last_time_from_rtc) == second)) {
      adjust_clock(time_from_rtc);
    }
    last_time_from_rtc = time_from_rtc;
  }
}

Time get_time() {
  return k_uptime_get() + uptime_offset_;
}

std::tuple<Time, Time> get_time_and_uptime() {
  auto uptime = k_uptime_get();
  return std::make_tuple(uptime + uptime_offset_, uptime);
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
