#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>

#define UART_NODE DT_CHOSEN(zephyr_console)

const struct device *uart_dev;

/* Thread stacks */
K_THREAD_STACK_DEFINE(tx_stack, 1024);
K_THREAD_STACK_DEFINE(rx_stack, 1024);

/* Thread control blocks */
struct k_thread tx_thread;
struct k_thread rx_thread;

void uart_send_string(const char *str)
{
    while (*str)
    {
        uart_poll_out(uart_dev, *str++);
    }
}

/* TX Thread */
void tx_task(void *a, void *b, void *c)
{
    while (1)
    {
        uart_send_string("Heartbeat\r\n");

        k_msleep(2000);
    }
}

/* RX Thread */
void rx_task(void *a, void *b, void *c)
{
    unsigned char ch;

    while (1)
    {
        if (uart_poll_in(uart_dev, &ch) == 0)
        {
            uart_poll_out(uart_dev, ch);
        }

        k_yield();
    }
}

int main(void)
{
    uart_dev = DEVICE_DT_GET(UART_NODE);

    if (!device_is_ready(uart_dev))
    {
        return 0;
    }

    k_thread_create(&tx_thread,
                    tx_stack,
                    K_THREAD_STACK_SIZEOF(tx_stack),
                    tx_task,
                    NULL, NULL, NULL,
                    5, 0, K_NO_WAIT);

    k_thread_create(&rx_thread,
                    rx_stack,
                    K_THREAD_STACK_SIZEOF(rx_stack),
                    rx_task,
                    NULL, NULL, NULL,
                    5, 0, K_NO_WAIT);

    return 0;
}
