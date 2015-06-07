#include "mbed.h"

I2C i2c(p28, p27);
DigitalOut led_1(LED1);
DigitalOut led_2(LED2);
DigitalOut led_3(LED3);
DigitalOut led_4(LED4);
RawSerial serial(USBTX, USBRX);

#define MPU_ADDRESS 0x68
#define MPU_GYRO_FS_250  0
#define MPU_GYRO_FS_500  1
#define MPU_GYRO_FS_1000 2
#define MPU_GYRO_FS_2000 3
#define MPU_REG_GYRO_CONFIG 0x1B
#define MPU_ACCEL_FS_2G  0
#define MPU_ACCEL_FS_4G  1
#define MPU_ACCEL_FS_8G  2
#define MPU_ACCEL_FS_16G 3
#define MPU_REG_ACCEL_CONFIG 0x1C
#define MPU_REG_ACCEL_XOUT_H 0x3B
#define MPU_REG_GYRO_XOUT_H  0x43
char read(char reg)
{
    char result = 0;
    i2c.write(MPU_ADDRESS << 1, &reg, 1, true);
    i2c.read(MPU_ADDRESS << 1, &result, 1);
    return result;
}

void read_multi(char reg, char *result, int count)
{
    i2c.write(MPU_ADDRESS << 1, &reg, 1, true);
    i2c.read(MPU_ADDRESS << 1, result, count);
}

void write(char reg, char data)
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

void read_acceleration(int16_t *x, int16_t *y, int16_t *z)
{
    // The accelerometer measurement registers contain
    // X_HI | X_LO | Y_HI | Y_LO | Z_HI | Z_LO
    // each 8 bits. We can read out each in succession.
    char data[6];
    read_multi(MPU_REG_ACCEL_XOUT_H, data, 6);
    *x = ((int16_t)data[0] << 8) | data[1];
    *y = ((int16_t)data[2] << 8) | data[3];
    *z = ((int16_t)data[4] << 8) | data[5];
}

void read_gyro(int16_t *x, int16_t *y, int16_t *z)
{
    char data[6];
    read_multi(MPU_REG_GYRO_XOUT_H, data, 6);
    *x = ((int16_t)data[0] << 8) | data[1];
    *y = ((int16_t)data[2] << 8) | data[3];
    *z = ((int16_t)data[4] << 8) | data[5];
}

struct SensorPacket
{
    union
    {
        uint8_t raw_bytes[18];
        struct
        {
            int16_t mag_x, mag_y, mag_z;
            int16_t gyro_x, gyro_y, gyro_z;
            int16_t accel_x, accel_y, accel_z;
        };
    };
};

int main()
{
    led_1 = 1;
    char data = 0;

    // Attempt to read WHO_AM_I register
    // The return value should be 1101000 (0x68)
    serial.putc(read(0x75));

    // Read the PWR_MGMT_1 register
    // DEVICE RESET | SLEEP | CYCLE | - | TEMP_DIS | CLKSEL[2:0]
    serial.putc(read(0x6B));

    // Write to the PWR_MGMT_1 register
    data = read(0x6B);
    data &= ~(1 << 6); // Disable sleep
    data |= (1 << 0);  // Choose x-component of gyro as clock
    write(0x6B, data);

    // Verify that it worked
    serial.putc(read(0x6B));

    // Set gyro range (in degrees per second)
    data = read(MPU_REG_GYRO_CONFIG);
    data |= (MPU_GYRO_FS_250 << 3);
    write(MPU_REG_GYRO_CONFIG, data);

    // Set accelerometer range (in multiples of g = 9.81m/ss)
    data = read(MPU_REG_ACCEL_CONFIG);
    data |= (MPU_ACCEL_FS_2G << 3);
    write(MPU_REG_ACCEL_CONFIG, data);

    wait_ms(1000);

    led_2 = 1;
    int handshake = 0xab;
    while (serial.getc() != handshake)
        wait_ms(100);
    led_3 = 1;
    while (1)
    {
        SensorPacket packet = {};
        read_acceleration(&packet.accel_x, &packet.accel_y, &packet.accel_z);
        read_gyro(&packet.gyro_x, &packet.gyro_y, &packet.gyro_z);
        for (size_t i = 0; i < sizeof(SensorPacket); i++)
            serial.putc(packet.raw_bytes[i]);
        while (serial.getc() != handshake)
            wait_ms(100);
    }

    led_1 = 0;
    led_2 = 0;
    led_3 = 0;
}