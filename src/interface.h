#ifndef IMUBAR_INTERFACE_H__
#define IMUBAR_INTERFACE_H__

#include "types.h"

namespace imubar {
namespace interface {

extern void initialize();
extern int read(uint8_t* data, size_t size);
extern int write(uint8_t const* data, size_t size);
extern void ping();

} // namespace interface
} // namespace imubar

#endif
