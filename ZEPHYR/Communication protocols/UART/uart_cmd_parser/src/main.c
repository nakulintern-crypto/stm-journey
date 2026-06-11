#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/gpio.h>

#include <string.h>

#define UART_NODE DT_CHOSEN(zephyr_console)
#define LED0_NODE DT_ALIAS(led0)

const struct device *uart_dev;

static const struct gpio_dt_spec led =
    GPIO_DT_SPEC_GET(LED0_NODE, gpios);

/* UART Message Queue */
K_MSGQ_DEFINE(uart_msgq,
              sizeof(char),
              50,
              4);

/* Command Thread */
K_THREAD_STACK_DEFINE(cmd_thread_stack, 1024);
struct k_thread cmd_thread_obj;

/* Send string over UART */
void uart_send_string(const char *str)
{
    while (*str)
    {
        uart_poll_out(uart_dev, *str++);
    }
}

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

/* Command Processing Thread */
void cmd_thread(void *a,
                void *b,
                void *c)
{
    char ch;

    char buffer[64];

    int index = 0;

    while (1)
    {
        k_msgq_get(&uart_msgq,
                   &ch,
                   K_FOREVER);

        /* Echo character */
        uart_poll_out(uart_dev, ch);

        if (ch == '\r' || ch == '\n')
        {
            buffer[index] = '\0';

            uart_send_string("\r\n");

            if (strcmp(buffer, "help") == 0)
            {
                uart_send_string(
                    "Commands:\r\n"
                    "help\r\n"
                    "status\r\n"
                    "led on\r\n"
                    "led off\r\n"
                    "led toggle\r\n"
                    "blink\r\n");
            }
            else if (strcmp(buffer, "status") == 0)
            {
                uart_send_string(
                    "System OK\r\n");
            }
            else if (strcmp(buffer, "led on") == 0)
            {
                gpio_pin_set_dt(&led, 1);

                uart_send_string(
                    "LED ON\r\n");
            }
            else if (strcmp(buffer, "led off") == 0)
            {
                gpio_pin_set_dt(&led, 0);

                uart_send_string(
                    "LED OFF\r\n");
            }
            else if (strcmp(buffer, "led toggle") == 0)
            {
                gpio_pin_toggle_dt(&led);

                uart_send_string(
                    "LED TOGGLED\r\n");
            }
            else if (strcmp(buffer, "blink") == 0)
            {
                for (int i = 0; i < 10; i++)
                {
                    gpio_pin_toggle_dt(&led);

                    k_msleep(200);
                }

                uart_send_string(
                    "BLINK DONE\r\n");
            }
            else if (strlen(buffer) > 0)
            {
                uart_send_string(
                    "Unknown Command\r\n");
            }

            index = 0;
        }
        else
        {
            if (index < sizeof(buffer) - 1)
            {
                buffer[index++] = ch;
            }
        }
    }
}

int main(void)
{
    uart_dev = DEVICE_DT_GET(UART_NODE);

    if (!device_is_ready(uart_dev))
    {
        return 0;
    }

    if (!gpio_is_ready_dt(&led))
    {
        return 0;
    }

    gpio_pin_configure_dt(
        &led,
        GPIO_OUTPUT_INACTIVE);

    uart_irq_callback_user_data_set(
        uart_dev,
        uart_isr,
        NULL);

    uart_irq_rx_enable(uart_dev);

    k_thread_create(
        &cmd_thread_obj,
        cmd_thread_stack,
        K_THREAD_STACK_SIZEOF(cmd_thread_stack),
        cmd_thread,
        NULL,
        NULL,
        NULL,
        5,
        0,
        K_NO_WAIT);

    uart_send_string(
        "\r\nUART Command Parser Ready\r\n");

    while (1)
    {
        k_sleep(K_SECONDS(1));
    }

    return 0;
}
