#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE 1024

K_THREAD_STACK_DEFINE(stack_a, STACK_SIZE);

struct k_thread thread_a;

void worker_thread(void *p1, void *p2, void *p3)
{
    for (int i = 1; i <= 5; i++)
    {
        printk("Worker running: %d\n", i);

        k_msleep(1000);
    }

    printk("Worker finished!\n");
}

int main(void)
{
    printk("Main started\n");

    k_thread_create(&thread_a,
                    stack_a,
                    STACK_SIZE,
                    worker_thread,
                    NULL, NULL, NULL,
                    5, 0, K_NO_WAIT);

    while (1)
    {
        printk("Main alive\n");
        k_msleep(2000);
    }
}
