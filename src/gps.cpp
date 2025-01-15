#include "gps.h"
#include "clock.h"
#include "errors.h"

#include <zephyr/drivers/gnss.h>
#include <zephyr/drivers/rtc.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/timeutil.h>

#include <time.h>

#define UART_GNSS DT_ALIAS(uartgnss)
#define GNSS_0 DT_NODELABEL(gnss_0)

LOG_MODULE_DECLARE(imubar);

static constexpr char const* mtk_only_rmcgga_and_baud_115200 =
    "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n"
    "$PMTK251,115200*1F\r\n";

static device const* const uart_gnss = DEVICE_DT_GET(UART_GNSS);
static device const* const gnss_ = DEVICE_DT_GET(GNSS_0);

static navigation_data data_{};
static bool has_fix_ = false;
static Time data_time_ = 0;

static void handle_gnss_data(device const* dev, gnss_data const* data) {
  static bool time_set = false;
  data_time_ = get_time();
  data_ = data->nav_data;
  has_fix_ = data->info.fix_status > 0;
  if (!time_set && has_fix_) {
    rtc_time caltime = {
        .tm_sec = data->utc.millisecond / 1000,
        .tm_min = data->utc.minute,
        .tm_hour = data->utc.hour,
        .tm_mday = data->utc.month_day,
        .tm_mon = data->utc.month - 1,
        .tm_year = 100 + data->utc.century_year,
        .tm_wday = -1,
        .tm_yday = -1,
        .tm_isdst = false,
        .tm_nsec = (data->utc.millisecond % 1000) * 1000000,
    };
    time_set = set_rtc(caltime);
  }
}

GNSS_DATA_CALLBACK_DEFINE(gnss_, handle_gnss_data);

namespace gnss {

bool has_fix() {
  return has_fix_;
}

bool has_data() {
  // Whether we have received data in the last 10 seconds
  return (get_time() - data_time_) < (10 * clock_scaler);
}

Position get_position() {
  return Position(1E-9 * data_.latitude, 1E-9 * data_.longitude);
}

Velocity get_velocity() {
  return Velocity(data_.speed, 1E-3 * data_.bearing);
}

} // namespace gnss

void initialize_gnss() {
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
