#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE 1024

K_THREAD_STACK_DEFINE(stack_a, STACK_SIZE);
K_THREAD_STACK_DEFINE(stack_b, STACK_SIZE);
K_THREAD_STACK_DEFINE(stack_c, STACK_SIZE);

struct k_thread thread_a;
struct k_thread thread_b;
struct k_thread thread_c;

void task_a(void *p1, void *p2, void *p3)
{
    while (1)
    {
        printk("HIGH PRIORITY THREAD A\n");
        k_msleep(1000);
    }
}

void task_b(void *p1, void *p2, void *p3)
{
    while (1)
    {
        printk("MEDIUM PRIORITY THREAD B\n");
        k_msleep(1000);
    }
}

void task_c(void *p1, void *p2, void *p3)
{
    while (1)
    {
        printk("LOW PRIORITY THREAD C\n");
        k_msleep(1000);
    }
}

int main(void)
{
    printk("Priority Demo Started\n");

    k_thread_create(&thread_a,
                    stack_a,
                    STACK_SIZE,
                    task_a,
                    NULL, NULL, NULL,
                    1,      /* Highest */
                    0,
                    K_NO_WAIT);

    k_thread_create(&thread_b,
                    stack_b,
                    STACK_SIZE,
                    task_b,
                    NULL, NULL, NULL,
                    5,
                    0,
                    K_NO_WAIT);

    k_thread_create(&thread_c,
                    stack_c,
                    STACK_SIZE,
                    task_c,
                    NULL, NULL, NULL,
                    10,     /* Lowest */
                    0,
                    K_NO_WAIT);

    while (1)
    {
        k_msleep(10000);
    }
}
