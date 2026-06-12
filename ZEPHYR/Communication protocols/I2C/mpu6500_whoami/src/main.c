#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>

#define I2C_NODE DT_NODELABEL(i2c1)

#define MPU6500_ADDR 0x68
#define WHO_AM_I_REG 0x75

const struct device *i2c_dev;

int main(void)
{
    uint8_t reg = WHO_AM_I_REG;
    uint8_t who_am_i = 0;

    int ret;

    printk("\nMPU6500 WHO_AM_I Test\n");

    i2c_dev = DEVICE_DT_GET(I2C_NODE);

    if (!device_is_ready(i2c_dev))
    {
        printk("I2C device not ready\n");
        return 0;
    }

    ret = i2c_write_read(
            i2c_dev,
            MPU6500_ADDR,
            &reg,
            1,
            &who_am_i,
            1);

    if (ret == 0)
    {
        printk("WHO_AM_I = 0x%02X\n",
               who_am_i);
    }
    else
    {
        printk("Read Failed (%d)\n",
               ret);
    }

    while (1)
    {
        k_sleep(K_SECONDS(5));
    }

    return 0;
}
