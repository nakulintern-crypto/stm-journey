#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE 1024

K_MSGQ_DEFINE(my_msgq,
              sizeof(int),
              10,
              4);

K_THREAD_STACK_DEFINE(producer_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(consumer_stack, STACK_SIZE);

struct k_thread producer_thread_data;
struct k_thread consumer_thread_data;

void producer_thread(void *p1, void *p2, void *p3)
{
    int value = 100;

    while (1)
    {
        printk("Producer: Sending %d\n", value);

        k_msgq_put(&my_msgq, &value, K_FOREVER);

        value++;

        k_msleep(2000);
    }
}

void consumer_thread(void *p1, void *p2, void *p3)
{
    int received;

    while (1)
    {
        k_msgq_get(&my_msgq, &received, K_FOREVER);

        printk("Consumer: Received %d\n", received);
    }
}

int main(void)
{
    k_thread_create(&producer_thread_data,
                    producer_stack,
                    STACK_SIZE,
                    producer_thread,
                    NULL, NULL, NULL,
                    5, 0, K_NO_WAIT);

    k_thread_create(&consumer_thread_data,
                    consumer_stack,
                    STACK_SIZE,
                    consumer_thread,
                    NULL, NULL, NULL,
                    5, 0, K_NO_WAIT);

    while (1)
    {
        k_msleep(10000);
    }
}
