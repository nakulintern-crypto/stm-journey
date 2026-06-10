#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE 1024

volatile int counter = 0;

struct k_mutex counter_mutex;

K_THREAD_STACK_DEFINE(stack_a, STACK_SIZE);
K_THREAD_STACK_DEFINE(stack_b, STACK_SIZE);

struct k_thread thread_a;
struct k_thread thread_b;

void task_a(void *p1, void *p2, void *p3)
{
    for (int i = 0; i < 1000; i++)
    {
        k_mutex_lock(&counter_mutex, K_FOREVER);

        counter++;

        k_mutex_unlock(&counter_mutex);
    }

    printk("Thread A Done\n");
}

void task_b(void *p1, void *p2, void *p3)
{
    for (int i = 0; i < 1000; i++)
    {
        k_mutex_lock(&counter_mutex, K_FOREVER);

        counter++;

        k_mutex_unlock(&counter_mutex);
    }

    printk("Thread B Done\n");
}

int main(void)
{
    k_mutex_init(&counter_mutex);

    k_thread_create(&thread_a,
                    stack_a,
                    STACK_SIZE,
                    task_a,
                    NULL, NULL, NULL,
                    5, 0, K_NO_WAIT);

    k_thread_create(&thread_b,
                    stack_b,
                    STACK_SIZE,
                    task_b,
                    NULL, NULL, NULL,
                    5, 0, K_NO_WAIT);

    k_msleep(1000);

    printk("Final Counter = %d\n", counter);

    while (1)
    {
        k_msleep(10000);
    }
}
