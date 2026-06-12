#ifndef MPU6500_H
#define MPU6500_H

#include <stdint.h>

struct imu_data
{
    int16_t ax;
    int16_t ay;
    int16_t az;

    int16_t gx;
    int16_t gy;
    int16_t gz;
};

int mpu6500_init(void);

int mpu6500_read_all(
    struct imu_data *imu);

#endif

