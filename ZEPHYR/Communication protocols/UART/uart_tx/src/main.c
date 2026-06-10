#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>

#define STACK_SIZE 1024

K_THREAD_STACK_DEFINE(uart_stack, STACK_SIZE);

struct k_thread uart_thread_data;

const struct device *uart_dev;

void uart_thread(void *p1, void *p2, void *p3)
{
    const char msg[] = "Hello from UART Thread\r\n";

    while (1)
    {
        for (int i = 0; msg[i] != '\0'; i++)
        {
            uart_poll_out(uart_dev, msg[i]);
        }

        k_msleep(1000);
    }
}

int main(void)
{
    uart_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));

    if (!device_is_ready(uart_dev))
    {
        return 0;
    }

    k_thread_create(&uart_thread_data,
                    uart_stack,
                    STACK_SIZE,
                    uart_thread,
                    NULL, NULL, NULL,
                    5, 0, K_NO_WAIT);

    while (1)
    {
        k_msleep(10000);
    }
}
