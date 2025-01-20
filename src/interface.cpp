#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "errors.h"
#include "interface.h"
#include "serial.h"

LOG_MODULE_DECLARE(imubar);

#ifndef INTERFACE_UART
#define INTERFACE_UART DT_ALIAS(uartinterface)
#endif

namespace imubar {
namespace interface {

static device const* const interface_uart = DEVICE_DT_GET(INTERFACE_UART);

RING_BUF_DECLARE(interface_input, 256);
RING_BUF_DECLARE(interface_output, 256);

static serial::UartData uart_data{&interface_input, &interface_output, "INTERFACE_UART"};

static constexpr char const* const ping_ = "INTFPING\r\n";
static constexpr char const* const pong_ = "INTFPONG\r\n";
static constexpr char const* const init_ = "INTFINIT\r\n";

int write(uint8_t const* data, size_t size) {
  return serial::write(interface_uart, data, size, &uart_data);
}

int read(uint8_t* data, size_t size) {
  return serial::read(interface_uart, data, size, &uart_data);
}

void ping() {
  write((uint8_t*)ping_, strlen(ping_));
}

void initialize() {
  if (!serial::initialize(interface_uart, &uart_data)) {
    error(2, "Interface UART not ready.");
  }

  serial::write(interface_uart, (uint8_t*)init_, strlen(init_), &uart_data);
}

} // namespace interface
} // namespace imubar
