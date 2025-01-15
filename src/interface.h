#ifndef IMUBAR_INTERFACE_H__
#define IMUBAR_INTERFACE_H__

#include "types.h"

extern void interface_init();
extern int interface_read(uint8_t* data, size_t size);
extern int interface_write(uint8_t const* data, size_t size);
extern void interface_ping();

#endif
