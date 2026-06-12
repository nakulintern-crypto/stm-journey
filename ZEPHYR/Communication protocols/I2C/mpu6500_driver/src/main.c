#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include "mpu6500.h"

int main(void)
{
    int16_t ax;
    int16_t ay;
    int16_t az;

    int16_t gx;
    int16_t gy;
    int16_t gz;

    if (mpu6500_init())
    {
        printk("MPU6500 Init Failed\n");
        return 0;
    }

    while (1)
    {
        if (!mpu6500_read_all(
                &ax,
                &ay,
                &az,
                &gx,
                &gy,
                &gz))
        {
            printk(
                "AX:%6d AY:%6d AZ:%6d | "
                "GX:%6d GY:%6d GZ:%6d\n",
                ax,
                ay,
                az,
                gx,
                gy,
                gz);
        }

        k_sleep(K_MSEC(500));
    }

    return 0;
}
