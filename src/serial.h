#ifndef IMUBAR_SERIAL_H__
#define IMUBAR_SERIAL_H__

#include <zephyr/sys/ring_buffer.h>

#include "types.h"

namespace imubar {
namespace serial {

struct UartData {
  ring_buf* input_buffer;
  ring_buf* output_buffer;
  char const* name;
};

extern bool initialize(device const* const uart, UartData const* uart_data);
extern int
write(device const* const uart, uint8_t const* data, size_t size, UartData* uart_data);
extern int read(device const* const uart, uint8_t* data, size_t size, UartData* uart_data);

} // namespace serial
} // namespace imubar

#endif
