#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"

#define UART_ID uart1
#define BAUD_RATE 115200

#define UART_TX_PIN 20
#define UART_RX_PIN 21

#define CH9120_TCPCS 17
#define CH9120_CFG0  18
#define CH9120_RSTI  19

int main()
{
    stdio_init_all();

    uart_init(UART_ID, BAUD_RATE);

    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    gpio_init(CH9120_TCPCS);
    gpio_set_dir(CH9120_TCPCS, GPIO_IN);

    gpio_init(CH9120_CFG0);
    gpio_set_dir(CH9120_CFG0, GPIO_OUT);

    gpio_init(CH9120_RSTI);
    gpio_set_dir(CH9120_RSTI, GPIO_OUT);

    // NORMAL MODE (NOT CONFIG MODE)
    gpio_put(CH9120_CFG0, 1);

    // Reset CH9120
    gpio_put(CH9120_RSTI, 0);
    sleep_ms(100);
    gpio_put(CH9120_RSTI, 1);
    sleep_ms(1000);

    printf("CH9120 Ready\n");

    while (1)
    {
        printf("TCPCS=%d\n", gpio_get(CH9120_TCPCS));

        if (gpio_get(CH9120_TCPCS) == 0)
        {
            uart_puts(UART_ID, "HELLO FROM RP2350\r\n");
        }

        sleep_ms(1000);
    }
}