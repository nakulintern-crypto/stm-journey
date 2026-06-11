#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>

#define UART_NODE DT_CHOSEN(zephyr_console)

const struct device *uart_dev;

int main(void)
{
    unsigned char c;

    uart_dev = DEVICE_DT_GET(UART_NODE);

    if (!device_is_ready(uart_dev))
    {
        return 0;
    }

    while (1)
    {
        if (uart_poll_in(uart_dev, &c) == 0)
        {
            uart_poll_out(uart_dev, c);
        }
    }
}
