#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>

#define STACK_SIZE 1024

const struct device *uart_dev;

K_THREAD_STACK_DEFINE(stack_a, STACK_SIZE);
K_THREAD_STACK_DEFINE(stack_b, STACK_SIZE);

struct k_thread thread_a;
struct k_thread thread_b;

void uart_task_a(void *p1, void *p2, void *p3)
{
    while (1)
    {
        const char msg[] = "AAAAA\r\n";

        for (int i = 0; msg[i] != '\0'; i++)
        {
            uart_poll_out(uart_dev, msg[i]);
        }

        k_msleep(1000);
    }
}

void uart_task_b(void *p1, void *p2, void *p3)
{
    while (1)
    {
        const char msg[] = "BBBBB\r\n";

        for (int i = 0; msg[i] != '\0'; i++)
        {
            uart_poll_out(uart_dev, msg[i]);
        }

        k_msleep(1500);
    }
}

int main(void)
{
    uart_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));

    if (!device_is_ready(uart_dev))
    {
        return 0;
    }

    k_thread_create(&thread_a,
                    stack_a,
                    STACK_SIZE,
                    uart_task_a,
                    NULL,NULL,NULL,
                    5,0,K_NO_WAIT);

    k_thread_create(&thread_b,
                    stack_b,
                    STACK_SIZE,
                    uart_task_b,
                    NULL,NULL,NULL,
                    5,0,K_NO_WAIT);

    while (1)
    {
        k_msleep(10000);
    }
}
