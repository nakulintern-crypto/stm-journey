#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE 1024

K_SEM_DEFINE(my_sem, 0, 10);

K_THREAD_STACK_DEFINE(worker_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(producer_stack, STACK_SIZE);

struct k_thread worker_thread_data;
struct k_thread producer_thread_data;

void worker_thread(void *p1, void *p2, void *p3)
{
    while (1)
    {
        k_sem_take(&my_sem, K_FOREVER);

        printk("Worker: Consumed one token\n");

        k_msleep(1000);
    }
}

void producer_thread(void *p1, void *p2, void *p3)
{
    while (1)
    {
        printk("\nProducer: Creating 5 events\n");

        for (int i = 0; i < 5; i++)
        {
            k_sem_give(&my_sem);

            printk("Producer: Token %d added\n", i + 1);
        }

        k_msleep(10000);
    }
}

int main(void)
{
    k_thread_create(&worker_thread_data,
                    worker_stack,
                    STACK_SIZE,
                    worker_thread,
                    NULL, NULL, NULL,
                    5, 0, K_NO_WAIT);

    k_thread_create(&producer_thread_data,
                    producer_stack,
                    STACK_SIZE,
                    producer_thread,
                    NULL, NULL, NULL,
                    5, 0, K_NO_WAIT);

    while (1)
    {
        k_msleep(10000);
    }
}
