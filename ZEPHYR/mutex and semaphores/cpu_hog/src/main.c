#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE 1024

K_THREAD_STACK_DEFINE(stack_a, STACK_SIZE);
K_THREAD_STACK_DEFINE(stack_b, STACK_SIZE);

struct k_thread thread_a;
struct k_thread thread_b;

void hog_task(void *a, void *b, void *c)
{
    while (1)
    {
        printk("A ");
    }
}

void sleepy_task(void *a, void *b, void *c)
{
    while (1)
    {
        printk("B\n");
        k_msleep(1000);
    }
}

int main(void)
{
    k_thread_create(&thread_a,
                    stack_a,
                    STACK_SIZE,
                    hog_task,
                    NULL, NULL, NULL,
                    5, 0, K_NO_WAIT);

    k_thread_create(&thread_b,
                    stack_b,
                    STACK_SIZE,
                    sleepy_task,
                    NULL, NULL, NULL,
                    5, 0, K_NO_WAIT);

    while (1)
    {
        k_msleep(10000);
    }
}
