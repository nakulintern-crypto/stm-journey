#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>

#define I2C_NODE DT_NODELABEL(i2c1)

#define MPU6500_ADDR 0x68
#define ACCEL_XOUT_H 0x3B

const struct device *i2c_dev;

int main(void)
{
    uint8_t reg;
    uint8_t data[14];

    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;

    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;

    int ret;

    printk("MPU6500 IMU TEST\n");

    i2c_dev = DEVICE_DT_GET(I2C_NODE);

    if (!device_is_ready(i2c_dev))
    {
        printk("I2C not ready\n");
        return 0;
    }

    while (1)
    {
        reg = ACCEL_XOUT_H;

        ret = i2c_write_read(
                i2c_dev,
                MPU6500_ADDR,
                &reg,
                1,
                data,
                14);

        if (ret == 0)
        {
            accel_x =
                (int16_t)((data[0] << 8) | data[1]);

            accel_y =
                (int16_t)((data[2] << 8) | data[3]);

            accel_z =
                (int16_t)((data[4] << 8) | data[5]);

            gyro_x =
                (int16_t)((data[8] << 8) | data[9]);

            gyro_y =
                (int16_t)((data[10] << 8) | data[11]);

            gyro_z =
                (int16_t)((data[12] << 8) | data[13]);

            printk(
                "AX:%6d AY:%6d AZ:%6d | "
                "GX:%6d GY:%6d GZ:%6d\n",
                accel_x,
                accel_y,
                accel_z,
                gyro_x,
                gyro_y,
                gyro_z);
        }
        else
        {
            printk("Read Failed\n");
        }

        k_sleep(K_MSEC(500));
    }

    return 0;
}
