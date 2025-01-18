#ifndef IMUBAR_WORK_H__
#define IMUBAR_WORK_H__

#include "types.h"

#include <functional>


namespace work {
using Work_item = std::function<void(void)>;

extern bool submit(Work_item work_item, int delay = 0);
extern void initialize();
} // namespace work

#endif
