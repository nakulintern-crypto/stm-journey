#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

/* Get LED from Device Tree */
#define LED0_NODE DT_ALIAS(led0)

static const struct gpio_dt_spec led =
    GPIO_DT_SPEC_GET(LED0_NODE, gpios);

int main(void)
{
    printk("GPIO Blink Started\n");

    if (!gpio_is_ready_dt(&led))
    {
        printk("LED not ready!\n");
        return 0;
    }

    gpio_pin_configure_dt(&led, GPIO_OUTPUT);

    while (1)
    {
        gpio_pin_toggle_dt(&led);

        printk("LED Toggled\n");

        k_msleep(1000);
    }
}
