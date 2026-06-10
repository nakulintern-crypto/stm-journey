#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

void timer_handler(struct k_timer *timer)
{
    printk("Timer Fired!\n");
}

K_TIMER_DEFINE(my_timer,
               timer_handler,
               NULL);

int main(void)
{
    printk("Starting Timer...\n");

    k_timer_start(&my_timer,
                  K_SECONDS(2),
                  K_SECONDS(2));

    while (1)
    {
        k_msleep(10000);
    }
}
