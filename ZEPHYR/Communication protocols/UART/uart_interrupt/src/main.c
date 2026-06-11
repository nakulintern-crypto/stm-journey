#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>

#define UART_NODE DT_CHOSEN(zephyr_console)

const struct device *uart_dev;

/* UART ISR */
void uart_isr(const struct device *dev, void *user_data)
{
    uint8_t ch;

    uart_irq_update(dev);

    if (uart_irq_rx_ready(dev))
    {
        uart_fifo_read(dev, &ch, 1);

        uart_fifo_fill(dev, &ch, 1);
    }
}

int main(void)
{
    uart_dev = DEVICE_DT_GET(UART_NODE);

    if (!device_is_ready(uart_dev))
    {
        return 0;
    }

    uart_irq_callback_user_data_set(
        uart_dev,
        uart_isr,
        NULL);

    uart_irq_rx_enable(uart_dev);

    while (1)
    {
        k_msleep(1000);
    }
}

