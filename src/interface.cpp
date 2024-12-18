#include <zephyr/kernel.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/ring_buffer.h>
#include <zephyr/logging/log.h>

#include "interface.h"
#include "errors.h"

LOG_MODULE_DECLARE(imubar);

#define UART_6 DT_NODELABEL(usart6)

static device const *const interface_uart = DEVICE_DT_GET(UART_6);

RING_BUF_DECLARE(interface_output, 1024);
RING_BUF_DECLARE(interface_input, 1024);

int interface_write(uint8_t const* data, size_t size) {
  uart_irq_tx_disable(interface_uart);
  auto result = ring_buf_put(&interface_input, data, size);
  uart_irq_tx_enable(interface_uart);
  return result;
}

int interface_read(uint8_t* data, size_t size) {
  uart_irq_rx_disable(interface_uart);
  auto result = ring_buf_get(&interface_output, data, size);
  uart_irq_rx_enable(interface_uart);
  return result;
}

void uart_cb(const struct device* dev, void* data) {
  static constexpr int const buf_size = 8;
  if(!uart_irq_update(interface_uart)) {
    return;
  }

  if (uart_irq_rx_ready(interface_uart) > 0) {
    uint8_t read_buf[buf_size];
    while (auto read = uart_fifo_read(interface_uart, read_buf, buf_size) > 0) {
      ring_buf_put(&interface_output, read_buf, read);
    }
  }

  if (uart_irq_tx_ready(interface_uart) > 0) {
    uint8_t* write_buf;
    auto to_write = ring_buf_get_claim(&interface_input, &write_buf, buf_size);
    if (to_write > 0) {
      auto written = uart_fifo_fill(interface_uart, write_buf, to_write);
      ring_buf_get_finish(&interface_input, written);
    }
    else {
      uart_irq_tx_disable(interface_uart);
    }
  }
}

void initialize_interface() {
  if (!device_is_ready(interface_uart)) {
    error(2, "UART 6 not ready.");
  }

  auto ret = uart_irq_callback_user_data_set(interface_uart, &uart_cb, nullptr);
  if (ret < 0) {
    error(2, "Failed to set UART 6 callback");
  }

  uart_irq_rx_enable(interface_uart);
}
