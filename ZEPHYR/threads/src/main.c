#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE 1024
#define PRIORITY 5

K_THREAD_STACK_DEFINE(stack_a, STACK_SIZE);
K_THREAD_STACK_DEFINE(stack_b, STACK_SIZE);

struct k_thread thread_a_data;
struct k_thread thread_b_data;

void thread_a(void *p1, void *p2, void *p3)
{
    while (1)
    {
        printk("Thread A\n");
        k_msleep(1000);
    }
}

void thread_b(void *p1, void *p2, void *p3)
{
    while (1)
    {
        printk("Thread B\n");
        k_msleep(500);
    }
}

int main(void)
{
    k_thread_create(&thread_a_data,
                    stack_a,
                    STACK_SIZE,
                    thread_a,
                    NULL, NULL, NULL,
                    PRIORITY,
                    0,
                    K_NO_WAIT);

    k_thread_create(&thread_b_data,
                    stack_b,
                    STACK_SIZE,
                    thread_b,
                    NULL, NULL, NULL,
                    PRIORITY,
                    0,
                    K_NO_WAIT);

    return 0;
}
