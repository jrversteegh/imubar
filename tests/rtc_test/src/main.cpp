#include <zephyr/kernel.h>
#include <zephyr/sys/timeutil.h>
#include <zephyr/drivers/rtc.h>
#include <zephyr/logging/log.h>

#include <time.h>

LOG_MODULE_REGISTER(rtc_test);

#define RTC_0 DT_NODELABEL(rtc_0)

static device const *const rtc = DEVICE_DT_GET(RTC_0);
static constexpr int clock_scaler = 1000;

static time_t uptime_offset_ = 0;

inline time_t rtc_time_to_time(rtc_time rtctime) {
  time_t time = timeutil_timegm(rtc_time_to_tm(&rtctime));
  return clock_scaler * time;
}

time_t get_time() {
  return k_uptime_get() + uptime_offset_;
}


void set_clock(time_t time) {
  LOG_INF("Set time: %lld", time);
  LOG_INF("Old offset: %lld", uptime_offset_);
  auto uptime = k_uptime_get();
  LOG_INF("Uptime: %lld", uptime);
  uptime_offset_ = time - uptime;
  LOG_INF("New offset: %lld", uptime_offset_);
}

void set_clock(rtc_time& rtctime) {
  set_clock(rtc_time_to_time(rtctime));
}

bool set_clock_from_rtc(bool exact=false) {
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

bool set_rtc(rtc_time &rtctime) {
  auto ret = rtc_set_time(rtc, &rtctime);
  if (ret < 0) {
    LOG_ERR("Failed to set RTC clock");
    return false;
  }
  set_clock(rtctime);
  return true;
}

time_t get_rtc() {
  rtc_time rtctime;
  auto ret = rtc_get_time(rtc, &rtctime);
  if (ret < 0) {
    LOG_ERR("Failed to get RTC time: %d", ret);
    return 0;
  }
  return rtc_time_to_time(rtctime);
}

int main() {
  LOG_INF("Starting...");
  LOG_INF("Init RTC...");
  if (!device_is_ready(rtc)) {
    LOG_ERR("Failed to initialize RTC");
    return 1;
  }
  LOG_INF("Done");
  k_msleep(1500);
  rtc_time starttime = {
        .tm_sec = 0,
        .tm_min = 0,
        .tm_hour = 0,
        .tm_mday = 1,
        .tm_mon = 0,
        .tm_year = 125,
        .tm_wday = -1,
        .tm_yday = -1,
        .tm_isdst = false,
        .tm_nsec = 0,
  };

  auto t = rtc_time_to_time(starttime);
  LOG_INF("Set RTC time: %lld...", t);

  if (!set_rtc(starttime)) {
    LOG_ERR("Failed to set RTC");
    return 1;
  }
  LOG_INF("Done");

  int i = 0;

  while (i < 200) {
    time_t time = get_time();
    time_t rtctime = get_rtc();
    if (i % 10 == 0) {
      LOG_INF("TIME: %lld", time);
      LOG_INF(" RTC: %lld", rtctime);
    }
    ++i;
    k_msleep(10);
  }

  k_msleep(500);

  LOG_INF("Reset RTC");
  // Check to see whether nanoseconds are ignored when setting.. They are!
  starttime.tm_nsec = 500000000;
  if (!set_rtc(starttime)) {
    LOG_ERR("Failed to set RTC");
    return 1;
  }
  LOG_INF("Done");

  i = 0;
  while (i < 200) {
    time_t time = get_time();
    time_t rtctime = get_rtc();
    if (i % 10 == 0) {
      LOG_INF("TIME: %lld", time);
      LOG_INF(" RTC: %lld", rtctime);
    }
    ++i;
    k_msleep(10);
  }

  uptime_offset_ =  0;
  set_clock_from_rtc(true);

  i = 0;
  while (i < 200) {
    time_t time = get_time();
    time_t rtctime = get_rtc();
    if (i % 10 == 0) {
      LOG_INF("TIME: %lld", time);
      LOG_INF(" RTC: %lld", rtctime);
    }
    ++i;
    k_msleep(10);
  }

  LOG_INF("Finished");
  return 0;
}
