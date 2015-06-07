#include "mpu.h"

#define MPU_ADDRESS 0x68
#define MPU_REG_GYRO_CONFIG 0x1B
#define MPU_REG_ACCEL_CONFIG 0x1C
#define MPU_REG_ACCEL_XOUT_H 0x3B
#define MPU_REG_GYRO_XOUT_H  0x43

I2C i2c(MPU_SDA, MPU_SCL);

char mpu_read(char reg)
{
    char result = 0;
    i2c.write(MPU_ADDRESS << 1, &reg, 1, true);
    i2c.read(MPU_ADDRESS << 1, &result, 1);
    return result;
}

void mpu_read_multi(char reg, char *result, int count)
{
    i2c.write(MPU_ADDRESS << 1, &reg, 1, true);
    i2c.read(MPU_ADDRESS << 1, result, count);
}

void mpu_write(char reg, char data)
{
    char cmd[2] = { reg, data };
    i2c.write(MPU_ADDRESS << 1, cmd, 2, true);
}

void swap_bytes(char *a, char *b)
{
    char temp = *a;
    *a = *b;
    *b = temp;
}

bool mpu_init(uint8_t accel_range, uint8_t gyro_range)
{
	// Attempt to read WHO_AM_I register
    // The return value should be 1101000 (0x68)
    char who_am_i = mpu_read(0x75);
    char data = 0;

    if (who_am_i != 0x68)
    	return false;

    // Write to the PWR_MGMT_1 register
    // DEVICE RESET | SLEEP | CYCLE | - | TEMP_DIS | CLKSEL[2:0]
    data = mpu_read(0x6B);
    data &= ~(1 << 6); // Disable sleep
    data |= (1 << 0);  // Choose x-component of gyro as clock
    mpu_write(0x6B, data);

    // Set gyro range (in degrees per second)
    data = mpu_read(MPU_REG_GYRO_CONFIG);
    data |= (gyro_range << 3);
    mpu_write(MPU_REG_GYRO_CONFIG, data);

    // Set accelerometer range (in multiples of g = 9.81m/ss)
    data = mpu_read(MPU_REG_ACCEL_CONFIG);
    data |= (accel_range << 3);
    mpu_write(MPU_REG_ACCEL_CONFIG, data);

    wait_ms(1000);
    return true;
}

void mpu_read_acceleration(int16_t *x, int16_t *y, int16_t *z)
{
    // The accelerometer measurement registers contain
    // X_HI | X_LO | Y_HI | Y_LO | Z_HI | Z_LO
    // each 8 bits. We can mpu_read out each in succession.
    char data[6];
    mpu_read_multi(MPU_REG_ACCEL_XOUT_H, data, 6);
    *x = ((int16_t)data[0] << 8) | data[1];
    *y = ((int16_t)data[2] << 8) | data[3];
    *z = ((int16_t)data[4] << 8) | data[5];
}

void mpu_read_gyro(int16_t *x, int16_t *y, int16_t *z)
{
    char data[6];
    mpu_read_multi(MPU_REG_GYRO_XOUT_H, data, 6);
    *x = ((int16_t)data[0] << 8) | data[1];
    *y = ((int16_t)data[2] << 8) | data[3];
    *z = ((int16_t)data[4] << 8) | data[5];
}