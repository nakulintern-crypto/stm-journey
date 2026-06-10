#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE 1024

struct sensor_data
{
    int ax;
    int ay;
    int az;
};

K_MSGQ_DEFINE(sensor_queue,
              sizeof(struct sensor_data),
              10,
              4);

K_THREAD_STACK_DEFINE(producer_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(consumer_stack, STACK_SIZE);

struct k_thread producer_thread_data;
struct k_thread consumer_thread_data;

void producer_thread(void *p1, void *p2, void *p3)
{
    int count = 0;

    while (1)
    {
        struct sensor_data data;

        data.ax = count;
        data.ay = count + 100;
        data.az = count + 200;

        printk("Producer: AX=%d AY=%d AZ=%d\n",
               data.ax,
               data.ay,
               data.az);

        k_msgq_put(&sensor_queue,
                   &data,
                   K_FOREVER);

        count++;

        k_msleep(2000);
    }
}

void consumer_thread(void *p1, void *p2, void *p3)
{
    struct sensor_data received;

    while (1)
    {
        k_msgq_get(&sensor_queue,
                   &received,
                   K_FOREVER);

        printk("Consumer: AX=%d AY=%d AZ=%d\n",
               received.ax,
               received.ay,
               received.az);
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
