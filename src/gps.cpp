#include "gps.h"
#include "error.h"

#include <zephyr/drivers/gnss.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/logging/log.h>

#define GNSS_0 DT_NODELABEL(gnss_0)

LOG_MODULE_DECLARE(imubar);

static constexpr char const *mtk_only_gprmc_and_baud_115200 =
    "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n$PMTK251,115200*"
    "1F\r\n";

static device const *const gnss = DEVICE_DT_GET(GNSS_0);

void initialize_gnss() {
  const struct device *uart = &gnss->uart;

  if (!device_is_ready(gnss)) {
    error(2, "GNSS not ready.");
  }

  // Get current UART config
  uart_config config;
  auto ret = uart_config_get(uart, &config);
  if (ret < 0) {
    error(2, "Failed to get GNSS uart config.");
  }

  // MTK init
  for (const char *c = mtk_only_gprmc_and_baud_115200; *c != '\0'; ++c) {
    uart_poll_out(uart, *c);
  }

  // Set UART config with updated baudrate
  config.baudrate = 115200;
  auto ret = uart_config(uart, &config);
  if (ret < 0) {
    error(2, "Failed to set GNSS uart config.");
  }
}
