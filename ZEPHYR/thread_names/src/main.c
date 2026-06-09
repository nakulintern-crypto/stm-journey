#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE 1024

K_THREAD_STACK_DEFINE(stack1, STACK_SIZE);
K_THREAD_STACK_DEFINE(stack2, STACK_SIZE);
K_THREAD_STACK_DEFINE(stack3, STACK_SIZE);

struct k_thread thread1;
struct k_thread thread2;
struct k_thread thread3;

void sensor_task(void *a, void *b, void *c)
{
    while (1)
    {
        printk("I am %s\n",
               k_thread_name_get(k_current_get()));

        k_msleep(1000);
    }
}

void uart_task(void *a, void *b, void *c)
{
    while (1)
    {
        printk("I am %s\n",
               k_thread_name_get(k_current_get()));

        k_msleep(1500);
    }
}

void network_task(void *a, void *b, void *c)
{
    while (1)
    {
        printk("I am %s\n",
               k_thread_name_get(k_current_get()));

        k_msleep(2000);
    }
}

int main(void)
{
    k_thread_create(&thread1,
                    stack1,
                    STACK_SIZE,
                    sensor_task,
                    NULL,NULL,NULL,
                    5,0,K_NO_WAIT);

    k_thread_create(&thread2,
                    stack2,
                    STACK_SIZE,
                    uart_task,
                    NULL,NULL,NULL,
                    5,0,K_NO_WAIT);

    k_thread_create(&thread3,
                    stack3,
                    STACK_SIZE,
                    network_task,
                    NULL,NULL,NULL,
                    5,0,K_NO_WAIT);

    k_thread_name_set(&thread1, "Sensor");
    k_thread_name_set(&thread2, "UART");
    k_thread_name_set(&thread3, "Network");

    while (1)
    {
        k_msleep(10000);
    }
}
