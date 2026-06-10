#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

#define LED0_NODE DT_ALIAS(led0)
#define BUTTON_NODE DT_ALIAS(sw0)

static const struct gpio_dt_spec led =
    GPIO_DT_SPEC_GET(LED0_NODE, gpios);

static const struct gpio_dt_spec button =
    GPIO_DT_SPEC_GET(BUTTON_NODE, gpios);

static struct gpio_callback button_cb_data;

void button_pressed(const struct device *dev,
                    struct gpio_callback *cb,
                    uint32_t pins)
{
    gpio_pin_toggle_dt(&led);

    printk("Button Interrupt!\n");
}

int main(void)
{
    printk("GPIO Interrupt Demo\n");

    if (!gpio_is_ready_dt(&led) ||
        !gpio_is_ready_dt(&button))
    {
        printk("GPIO not ready\n");
        return 0;
    }

    gpio_pin_configure_dt(&led, GPIO_OUTPUT);

    gpio_pin_configure_dt(&button, GPIO_INPUT);

    gpio_pin_interrupt_configure_dt(
        &button,
        GPIO_INT_EDGE_TO_ACTIVE);

    gpio_init_callback(
        &button_cb_data,
        button_pressed,
        BIT(button.pin));

    gpio_add_callback(
        button.port,
        &button_cb_data);

    printk("Press User Button...\n");

    while (1)
    {
        k_msleep(10000);
    }
}
