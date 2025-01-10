#ifndef IMUBAR_SERIAL_H__
#define IMUBAR_SERIAL_H__

#include <zephyr/sys/ring_buffer.h>

#include "types.h"

struct UartData {
  ring_buf* input_buffer;
  ring_buf* output_buffer;
  const char* name;
};


extern bool serial_init(device const* const uart, UartData const* uart_data);
extern int serial_write(device const* const uart, uint8_t const *data, size_t size, UartData* uart_data);
extern int serial_read(device const* const uart, uint8_t *data, size_t size, UartData* uart_data);

#endif
