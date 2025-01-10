#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "errors.h"
#include "interface.h"
#include "serial.h"

LOG_MODULE_DECLARE(imubar);

static device const *const interface_uart =
    DEVICE_DT_GET(DT_NODELABEL(INTERFACE_UART));

RING_BUF_DECLARE(interface_input, 1024);
RING_BUF_DECLARE(interface_output, 1024);

static UartData uart_data{&interface_input, &interface_output, "INTERFACE_UART"};

int interface_write(uint8_t const *data, size_t size) {
  return serial_write(interface_uart, data, size, &uart_data);
}

int interface_read(uint8_t *data, size_t size) {
  return serial_read(interface_uart, data, size, &uart_data);
}

void initialize_interface() {
  if (!serial_init(interface_uart, &uart_data)) {
    error(2, "Interface UART not ready.");
  }
}
