#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>

#define UART_NODE DT_CHOSEN(zephyr_console)

const struct device *uart_dev;

/* Queue of chars */
K_MSGQ_DEFINE(uart_msgq,
              sizeof(char),
              20,
              4);

/* Thread stack */
K_THREAD_STACK_DEFINE(process_thread_stack, 1024);

struct k_thread process_thread_obj;

/* UART ISR */
void uart_isr(const struct device *dev,
              void *user_data)
{
    char ch;

    uart_irq_update(dev);

    if (uart_irq_rx_ready(dev))
    {
        uart_fifo_read(dev, &ch, 1);

        k_msgq_put(&uart_msgq,
                   &ch,
                   K_NO_WAIT);
    }
}

/* Processing Thread */
void process_thread(void *a,
                    void *b,
                    void *c)
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

    uart_irq_callback_user_data_set(
        uart_dev,
        uart_isr,
        NULL);

    uart_irq_rx_enable(uart_dev);

    k_thread_create(
        &process_thread_obj,
        process_thread_stack,
        K_THREAD_STACK_SIZEOF(process_thread_stack),
        process_thread,
        NULL, NULL, NULL,
        5, 0, K_NO_WAIT);

    while (1)
    {
        k_sleep(K_SECONDS(1));
    }
}
