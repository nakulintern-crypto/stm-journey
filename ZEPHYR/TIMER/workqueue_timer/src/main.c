#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

void work_handler(struct k_work *work)
{
    printk("Work Queue: Doing Work...\n");

    k_msleep(1000);

    printk("Work Queue: Work Finished\n");
}

K_WORK_DEFINE(my_work, work_handler);

void timer_handler(struct k_timer *timer)
{
    printk("Timer Fired\n");

    k_work_submit(&my_work);
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
