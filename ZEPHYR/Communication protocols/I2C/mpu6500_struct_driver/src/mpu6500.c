#include "mpu6500.h"

#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>

#define I2C_NODE DT_NODELABEL(i2c1)

#define MPU6500_ADDR 0x68
#define ACCEL_XOUT_H 0x3B

static const struct device *i2c_dev;

int mpu6500_init(void)
{
    i2c_dev = DEVICE_DT_GET(I2C_NODE);

    if (!device_is_ready(i2c_dev))
    {
        return -1;
    }

    return 0;
}

int mpu6500_read_all(struct imu_data *imu)
{
    uint8_t reg = ACCEL_XOUT_H;

    uint8_t data[14];

    int ret;

    ret = i2c_write_read(
            i2c_dev,
            MPU6500_ADDR,
            &reg,
            1,
            data,
            14);

    if (ret)
    {
        return ret;
    }

    imu->ax = (int16_t)((data[0] << 8) | data[1]);
    imu->ay = (int16_t)((data[2] << 8) | data[3]);
    imu->az = (int16_t)((data[4] << 8) | data[5]);

    imu->gx = (int16_t)((data[8] << 8) | data[9]);
    imu->gy = (int16_t)((data[10] << 8) | data[11]);
    imu->gz = (int16_t)((data[12] << 8) | data[13]);

    return 0;
}

