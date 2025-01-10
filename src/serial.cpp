#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "serial.h"

LOG_MODULE_DECLARE(imubar);

static void uart_cb(const struct device *dev, void *data) {
  static constexpr int const buf_size = 8;
  UartData* uart_data = (UartData*)data;

  if (!uart_irq_update(dev)) {
    return;
  }

  if (uart_irq_rx_ready(dev) > 0) {
    uint8_t read_buf[buf_size];
    while (auto read = uart_fifo_read(dev, read_buf, buf_size) > 0) {
      ring_buf_put(uart_data->input_buffer, read_buf, read);
    }
  }

  if (uart_irq_tx_ready(dev) > 0) {
    uint8_t *write_buf;
    auto to_write = ring_buf_get_claim(uart_data->output_buffer, &write_buf, buf_size);
    if (to_write > 0) {
      auto written = uart_fifo_fill(dev, write_buf, to_write);
      ring_buf_get_finish(uart_data->output_buffer, written);
    } else {
      uart_irq_tx_disable(dev);
    }
  }
}

bool serial_init(device const* const uart, UartData const* uart_data) {
  if (!device_is_ready(uart)) {
    LOG_ERR("%s not ready.", uart_data->name);
    return false;
  }

  auto ret = uart_irq_callback_user_data_set(uart, &uart_cb, (void*)uart_data);
  if (ret < 0) {
    LOG_ERR("Failed to set %s callback", uart_data->name);
    return false;
  }

  uart_irq_rx_enable(uart);
  return true;
}

int serial_write(device const* const uart, uint8_t const *data, size_t size, UartData* uart_data) {
  uart_irq_tx_disable(uart);
  auto result = ring_buf_put(uart_data->output_buffer, data, size);
  uart_irq_tx_enable(uart);
  return result;
}

int serial_read(device const* const uart, uint8_t *data, size_t size, UartData* uart_data) {
  uart_irq_rx_disable(uart);
  auto result = ring_buf_get(uart_data->input_buffer, data, size);
  uart_irq_rx_enable(uart);
  return result;
}

