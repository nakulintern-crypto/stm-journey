#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE 1024

volatile int counter = 0;

K_THREAD_STACK_DEFINE(stack_a, STACK_SIZE);
K_THREAD_STACK_DEFINE(stack_b, STACK_SIZE);

struct k_thread thread_a;
struct k_thread thread_b;

void increment_task(void *a, void *b, void *c)
{
    while (1)
    {
        counter++;
        printk("Incremented Counter = %d\n", counter);

        k_msleep(1000);
    }
}

void print_task(void *a, void *b, void *c)
{
    while (1)
    {
        printk("Current Counter = %d\n", counter);

        k_msleep(500);
    }
}

int main(void)
{
    printk("Shared Counter Demo\n");

    k_thread_create(&thread_a,
                    stack_a,
                    STACK_SIZE,
                    increment_task,
                    NULL, NULL, NULL,
                    5, 0, K_NO_WAIT);

    k_thread_create(&thread_b,
                    stack_b,
                    STACK_SIZE,
                    print_task,
                    NULL, NULL, NULL,
                    5, 0, K_NO_WAIT);

    while (1)
    {
        k_msleep(10000);
    }
}
