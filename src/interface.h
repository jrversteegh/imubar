#ifndef IMUBAR_INTERFACE_H__
#define IMUBAR_INTERFACE

#include "types.h"

extern void initialize_interface();
extern int interface_read(uint8_t* data, size_t size);
extern int interface_write(uint8_t const* data, size_t size);

#endif
