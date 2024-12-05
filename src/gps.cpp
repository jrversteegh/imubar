#include "gps.h"

#include <zephyr/logging/log.h>

#define GNSS_0 DT_NODELABEL(gnss_0)

LOG_MODULE_DECLARE(imubar);

static constexpr char const*set_mtk_baud_115200 = "$PMTK251,115200*1F\r\n";
static constexpr char const*set_mtk_only_gprmc =
    "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n";

static device const *const gnss = DEVICE_DT_GET(GNSS_0);
