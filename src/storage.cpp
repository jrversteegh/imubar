#include "storage.h"
#include "errors.h"

#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(imubar);

#define SPI_FLASH DT_NODELABEL(spi_flash_0)

namespace imubar {

static device const* const spi_flash = DEVICE_DT_GET(SPI_FLASH);

namespace storage {

void initialize() {
  if (!device_is_ready(spi_flash)) {
    error(2, "SPI FLASH not ready.");
  }
  auto page_count = flash_get_page_count(spi_flash);
  LOG_DBG("Flash has %d pages", page_count);
  flash_pages_info info;
  flash_get_page_info_by_offs(spi_flash, 0x0, &info);
  LOG_DBG(
      "First page at %ld with index %d and size %d", info.start_offset, info.index, info.size);
  flash_get_page_info_by_offs(spi_flash, 0xFFFFFE, &info);
  LOG_DBG(
      "Last page at %ld with index %d and size %d", info.start_offset, info.index, info.size);
}

} // namespace storage
} // namespace imubar
