#ifndef IMUBAR_STORAGE_H__
#define IMUBAR_STORAGE_H__

#include <zephyr/device.h>
#include <zephyr/drivers/flash.h>

#include "types.h"

namespace imubar {
namespace storage {

extern void initialize();

} // namespace storage
} // namespace imubar

#endif
