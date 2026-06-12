#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include "mpu6500.h"

int main(void)
{
    struct imu_data imu;

    if (mpu6500_init())
    {
        printk("MPU6500 Init Failed\n");
        return 0;
    }

    printk("MPU6500 Driver Started\n");

    while (1)
    {
        if (!mpu6500_read_all(&imu))
        {
            printk(
                "AX:%6d AY:%6d AZ:%6d | "
                "GX:%6d GY:%6d GZ:%6d\n",
                imu.ax,
                imu.ay,
                imu.az,
                imu.gx,
                imu.gy,
                imu.gz);
        }

        k_sleep(K_MSEC(500));
    }

    return 0;
}
