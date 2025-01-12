#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/ring_buffer.h>


LOG_MODULE_REGISTER(xiao_uart_test);

#define LED_NODE DT_ALIAS(led0)
#define SERIAL_NODE DT_ALIAS(serial0)

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);
static device const *const uart = DEVICE_DT_GET(SERIAL_NODE);

RING_BUF_DECLARE(uart_input, 128);
RING_BUF_DECLARE(uart_output, 128);

struct UartData {
  ring_buf* input_buffer;
  ring_buf* output_buffer;
  const char* name;
};


UartData uart_data{&uart_input, &uart_output, "uart"};

static void uart_cb(const struct device *dev, void *data) {
  static constexpr int const buf_size = 32;
  UartData* uart_data = (UartData*)data;

  if (!uart_irq_update(dev)) {
    return;
  }

  if (uart_irq_rx_ready(dev) > 0) {
    uint8_t read_buf[buf_size];
    int read = 0;
    while ((read = uart_fifo_read(dev, read_buf, buf_size)) > 0) {
      ring_buf_put(uart_data->input_buffer, read_buf, read);
    }
  }

  if (uart_irq_tx_ready(dev) > 0) {
    uint8_t *write_buf;
    auto to_write = ring_buf_get_claim(uart_data->output_buffer, &write_buf, buf_size);
    if (to_write > 0) {
      LOG_INF("To write: %d %c", to_write, write_buf[0]);
      auto written = uart_fifo_fill(dev, write_buf, to_write);
      LOG_INF("Written: %d", written);
      ring_buf_get_finish(uart_data->output_buffer, written);
    } else {
      uart_irq_tx_disable(dev);
    }
  }
}

bool init_uart(device const* const uart, UartData const* uart_data) {
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

int uart_write(device const* const uart, uint8_t const *data, size_t size, UartData* uart_data) {
  uart_irq_tx_disable(uart);
  auto result = ring_buf_put(uart_data->output_buffer, data, size);
  uart_irq_tx_enable(uart);
  return result;
}

int uart_read(device const* const uart, uint8_t *data, size_t size, UartData* uart_data) {
  uart_irq_rx_disable(uart);
  auto result = ring_buf_get(uart_data->input_buffer, data, size);
  uart_irq_rx_enable(uart);
  return result;
}

int main() {
  if (!gpio_is_ready_dt(&led)) {
    LOG_ERR("Led0 not ready");
    return 0;
  }

  auto ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
  if (ret < 0) {
    LOG_ERR("Failed to configure led0");
    return 0;
  }
/*
  if (!init_uart(uart, &uart_data)) {
    return -1;
  }
  */

  int i = 0;
  LOG_INF("Starting loop");
  while (true) {
    ++i;
    if (i % 200 == 0) {
      LOG_INF("Toggle led...");
      gpio_pin_toggle_dt(&led);
      LOG_INF("... done");
      /*
      LOG_INF("Writing uart...");
      char buf[32];
      snprintf(buf, sizeof(buf), "Hello from uart: %d\r\n", i);
      uart_write(uart, (uint8_t const*)buf, strlen(buf), &uart_data);
      char read_buf[32];
      if (int l = uart_read(uart, (uint8_t*)read_buf, sizeof(read_buf), &uart_data)) {
        snprintf(buf, sizeof(buf), "Got %d chars:\r\n", l);
        uart_write(uart, (uint8_t const*)buf, strlen(buf), &uart_data);
        uart_write(uart, (uint8_t const*)read_buf, l, &uart_data);
      }
      */
      LOG_INF("... done");
    }
    k_msleep(10);
  }
  return 0;
}
