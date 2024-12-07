#include <zephyr/kernel.h>
#include <zephyr/sys/timeutil.h>
#include <zephyr/logging/log.h>
#include <time.h>


#include "clock.h"
#include "errors.h"

LOG_MODULE_DECLARE(imubar);

#define RTC_0 DT_NODELABEL(rtc_0)

static device const *const rtc = DEVICE_DT_GET(RTC_0);

static time_t _uptime_offset = 0;

bool set_rtc(rtc_time const& time) {
  auto ret = rtc_set_time(rtc, &time);
  if (ret < 0) {
    LOG_ERR("Failed to set RTC clock");
    return false;
  }
  return true;
}

void set_clock(Time time) {
  _uptime_offset = time - k_uptime_get();
}

Time get_time() {
  static Time seen = 0;
  Time result = k_uptime_get() + _uptime_offset;
  if (result < seen) {
    // Don't go back in time: return the latest time seen
    return seen;
  }
  else {
    seen = result;
    return result;
  }
}

void initialize_clock() {
  if (!device_is_ready(rtc)) {
    error(2, "RTC not ready.");
  }

  rtc_time rtctime;
  auto ret = rtc_get_time(rtc, &rtctime);
  if (ret < 0) {
    error(2, "Failed to get RTC time: %d", ret);
  }

  Time time = timeutil_timegm(rtc_time_to_tm(&rtctime));
  set_clock(1000LL * time);
}
