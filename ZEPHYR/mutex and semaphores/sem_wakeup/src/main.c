#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE 1024

K_SEM_DEFINE(my_sem, 0, 1);

K_THREAD_STACK_DEFINE(stack_a, STACK_SIZE);
K_THREAD_STACK_DEFINE(stack_b, STACK_SIZE);

struct k_thread thread_a;
struct k_thread thread_b;

void waiting_thread(void *p1, void *p2, void *p3)
{
    printk("Thread A: Waiting for semaphore...\n");

    k_sem_take(&my_sem, K_FOREVER);

    printk("Thread A: Semaphore received!\n");
}

void signaling_thread(void *p1, void *p2, void *p3)
{
    printk("Thread B: Sleeping 5 seconds...\n");

    k_msleep(5000);

    printk("Thread B: Giving semaphore\n");

    k_sem_give(&my_sem);
}

int main(void)
{
    k_thread_create(&thread_a,
                    stack_a,
                    STACK_SIZE,
                    waiting_thread,
                    NULL, NULL, NULL,
                    5, 0, K_NO_WAIT);

    k_thread_create(&thread_b,
                    stack_b,
                    STACK_SIZE,
                    signaling_thread,
                    NULL, NULL, NULL,
                    5, 0, K_NO_WAIT);

    while (1)
    {
        k_msleep(10000);
    }
}
