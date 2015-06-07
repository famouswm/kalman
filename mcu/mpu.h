#ifndef _mpu_h_
#define _mpu_h_
#ifndef MPU_SDA
#define MPU_SDA p28
#endif
#ifndef MPU_SCL
#define MPU_SCL p27
#endif
#define MPU_GYRO_FS_250  0
#define MPU_GYRO_FS_500  1
#define MPU_GYRO_FS_1000 2
#define MPU_GYRO_FS_2000 3
#define MPU_ACCEL_FS_2G  0
#define MPU_ACCEL_FS_4G  1
#define MPU_ACCEL_FS_8G  2
#define MPU_ACCEL_FS_16G 3
#include "mbed.h"

bool mpu_init(uint8_t accel_range, uint8_t gyro_range);
void mpu_read_acceleration(int16_t *x, int16_t *y, int16_t *z);
void mpu_read_gyro(int16_t *x, int16_t *y, int16_t *z);

#endif