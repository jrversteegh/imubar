#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "errors.h"
#include "interface.h"
#include "serial.h"

LOG_MODULE_DECLARE(imubar);

#ifndef INTERFACE_UART
#define INTERFACE_UART DT_ALIAS(uartinterface)
#endif

static device const* const interface_uart = DEVICE_DT_GET(INTERFACE_UART);

RING_BUF_DECLARE(interface_input, 256);
RING_BUF_DECLARE(interface_output, 256);

static UartData uart_data{&interface_input, &interface_output, "INTERFACE_UART"};

static constexpr char const* const ping = "INTFPING\r\n";
static constexpr char const* const pong = "INTFPONG\r\n";

int interface_write(uint8_t const* data, size_t size) {
  return serial_write(interface_uart, data, size, &uart_data);
}

int interface_read(uint8_t* data, size_t size) {
  return serial_read(interface_uart, data, size, &uart_data);
}

void interface_ping() {
  static int const ping_len = strlen(ping);
  interface_write((uint8_t*)ping, ping_len);
}

void interface_init() {
  if (!serial_init(interface_uart, &uart_data)) {
    error(2, "Interface UART not ready.");
  }

  serial_write(interface_uart, (uint8_t*)"\r\nINTFINIT\r\n", 12, &uart_data);
}
