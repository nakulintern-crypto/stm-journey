#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>

#define UART_NODE DT_CHOSEN(zephyr_console)

const struct device *uart_dev;

/* Queue stores chars */
K_MSGQ_DEFINE(uart_msgq,
              sizeof(char),
              10,
              4);

K_THREAD_STACK_DEFINE(rx_stack, 1024);
K_THREAD_STACK_DEFINE(proc_stack, 1024);

struct k_thread rx_thread;
struct k_thread proc_thread;

void uart_send_string(const char *str)
{
    while (*str)
    {
        uart_poll_out(uart_dev, *str++);
    }
}

/* Producer */
void rx_task(void *a, void *b, void *c)
{
    char ch;

    while (1)
    {
        if (uart_poll_in(uart_dev, &ch) == 0)
        {
            k_msgq_put(&uart_msgq,
                       &ch,
                       K_NO_WAIT);
        }

        k_yield();
    }
}

/* Consumer */
void process_task(void *a, void *b, void *c)
{
    char ch;

    while (1)
    {
        k_msgq_get(&uart_msgq,
                   &ch,
                   K_FOREVER);

        uart_poll_out(uart_dev, ch);
    }
}

int main(void)
{
    uart_dev = DEVICE_DT_GET(UART_NODE);

    if (!device_is_ready(uart_dev))
    {
        return 0;
    }

    k_thread_create(&rx_thread,
                    rx_stack,
                    K_THREAD_STACK_SIZEOF(rx_stack),
                    rx_task,
                    NULL, NULL, NULL,
                    5, 0, K_NO_WAIT);

    k_thread_create(&proc_thread,
                    proc_stack,
                    K_THREAD_STACK_SIZEOF(proc_stack),
                    process_task,
                    NULL, NULL, NULL,
                    5, 0, K_NO_WAIT);

    return 0;
}
