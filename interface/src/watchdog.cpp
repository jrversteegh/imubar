#include <zephyr/kernel.h>
#include <zephyr/drivers/watchdog.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>

#include "errors.h"
#include "watchdog.h"

LOG_MODULE_DECLARE(imubar);

namespace imubar {
namespace watchdog {

#define WDT_MAX_WINDOW 1000U
#define WDT_MIN_WINDOW 0U

int wdt_channel_id = -1;
const struct device *const wdt = DEVICE_DT_GET(DT_ALIAS(watchdog0));

static void wdt_callback(const struct device *wdt_dev, int channel_id)
{
  static bool handled_event = false;

  if (handled_event) {
    return;
  }

  wdt_feed(wdt_dev, channel_id);

  LOG_WRN("Watchdog trigger\n");
  handled_event = true;
}

void initialize() {
  if (!device_is_ready(wdt)) {
    error(2, "Watchdog device not ready.");
  }

  struct wdt_timeout_cfg wdt_config{};
  /* Reset SoC when watchdog timer expires. */
  wdt_config.flags = WDT_FLAG_RESET_SOC,

  /* Expire watchdog after max window */
  wdt_config.window.min = WDT_MIN_WINDOW,
  wdt_config.window.max = WDT_MAX_WINDOW,
  wdt_config.callback = wdt_callback,

  wdt_channel_id = wdt_install_timeout(wdt, &wdt_config);
  if (wdt_channel_id == -ENOTSUP) {
    wdt_config.callback = NULL;
    wdt_channel_id = wdt_install_timeout(wdt, &wdt_config);
  }

  if (wdt_channel_id < 0) {
    error(2, "Watchdog install error");
    return;
  }

  auto ret = wdt_setup(wdt, WDT_OPT_PAUSE_HALTED_BY_DBG);
  if (ret < 0) {
    error(2, "Watchdog setup error: %d", ret);
    return;
  }

  LOG_INF("Successful watchdog init");
}

void feed() {
  wdt_feed(wdt, wdt_channel_id);
}

}  // namespace watchdog
}  // namespace imubar
