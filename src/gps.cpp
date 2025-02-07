#include <zephyr/drivers/gnss.h>
#include <zephyr/drivers/rtc.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/logging/log.h>

#include "clock.h"
#include "errors.h"
#include "gps.h"

#define UART_GNSS DT_ALIAS(uartgnss)
#define GNSS_0 DT_NODELABEL(gnss_0)

LOG_MODULE_DECLARE(imubar);

namespace imubar {
namespace gnss {

static constexpr char const* mtk_only_rmcgga_and_baud_115200 =
    "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n"
    "$PMTK251,115200*1F\r\n";

static device const* const uart_gnss = DEVICE_DT_GET(UART_GNSS);
static device const* const gnss_ = DEVICE_DT_GET(GNSS_0);

static navigation_data data_{};
static bool has_fix_ = false;
static Time data_time_ = 0;

constexpr int gps_reception_delay = 200_rtcms;

static void handle_gnss_data(device const* dev, gnss_data const* data) {
  static int time_set_day = -1;
  data_time_ = clock::get_time();
  data_ = data->nav_data;
  has_fix_ = data->info.fix_status > 0;
  if (has_fix_) {
    auto const& utc = data->utc;
    auto seconds = utc.millisecond / 1000;
    auto nanos = (utc.millisecond % 1000) * 1'000'000 + gps_reception_delay;
    seconds += nanos / 1'000'000'000;
    nanos = nanos % 1'000'000'000;
    //  Do clock setting and adjusting only on 10s boundaries
    if ((seconds % 10) == 0) {
      // The GPS data can have between a 50ms to 400ms delay to be received.
      // We'd need to generate an interrupt from the GPS PPS signal to do better.
      // The PPS is not broken out on the board we have unfortunately.
      // We'll guess a 250ms reception delay
      rtc_time gpstime = {
          .tm_sec = seconds,
          .tm_min = utc.minute,
          .tm_hour = utc.hour,
          .tm_mday = utc.month_day,
          .tm_mon = utc.month - 1,
          .tm_year = 100 + utc.century_year,
          .tm_wday = -1,
          .tm_yday = -1,
          .tm_isdst = false,
          .tm_nsec = nanos,
      };
      if (utc.month_day != time_set_day) {
        if (clock::set_rtc(gpstime, time_set_day < 0)) {
          time_set_day = utc.month_day;
        }
      }
    }
  }
}

GNSS_DATA_CALLBACK_DEFINE(gnss_, handle_gnss_data);

bool has_fix() {
  return has_fix_;
}

bool has_data() {
  // Whether we have received data in the last 10 seconds
  return (clock::get_time() - data_time_) < (10 * clock_scaler);
}

Position get_position() {
  return Position(1E-9 * data_.latitude, 1E-9 * data_.longitude);
}

Velocity get_velocity() {
  return Velocity(data_.speed, 1E-3 * data_.bearing);
}


void initialize() {
  if (!device_is_ready(gnss_)) {
    error(2, "GNSS not ready.");
  }

  // Get current UART config
  uart_config config;
  auto ret = uart_config_get(uart_gnss, &config);
  if (ret < 0) {
    error(2, "Failed to get GNSS uart config.");
  }

  // MTK init
  for (char const* c = mtk_only_rmcgga_and_baud_115200; *c != '\0'; ++c) {
    uart_poll_out(uart_gnss, *c);
  }

  k_msleep(500);

  // Set UART config with updated baudrate
  config.baudrate = 115200;
  ret = uart_configure(uart_gnss, &config);
  if (ret < 0) {
    error(2, "Failed to set GNSS uart config.");
  }
}

} // namespace gnss
} // namespace imubar
