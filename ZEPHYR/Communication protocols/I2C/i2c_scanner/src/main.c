#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>

#define I2C_NODE DT_NODELABEL(i2c1)

const struct device *i2c_dev;

int main(void)
{
    int ret;

    i2c_dev = DEVICE_DT_GET(I2C_NODE);

    if (!device_is_ready(i2c_dev))
    {
        printk("I2C device not ready\n");
        return 0;
    }

    printk("I2C Scanner Started\n");

    while (1)
    {
        printk("\nScanning...\n");

        for (uint8_t addr = 0x08;
             addr <= 0x77;
             addr++)
        {
            ret = i2c_write(i2c_dev,
                            NULL,
                            0,
                            addr);

            if (ret == 0)
            {
                printk("Found device at 0x%02X\n",
                       addr);
            }
        }

        k_sleep(K_SECONDS(5));
    }

    return 0;
}
