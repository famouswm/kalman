#include "mbed.h"

I2C i2c(p28, p27);
DigitalOut led_1(LED1);
DigitalOut led_2(LED2);
DigitalOut led_3(LED3);
DigitalOut led_4(LED4);
RawSerial serial(USBTX, USBRX);

#define ADDRESS 0x68
char read(char reg)
{
    char result = 0;
    i2c.write(ADDRESS << 1, &reg, 1, true);
    i2c.read(ADDRESS << 1, &result, 1);
    return result;
}

void read_multi(char reg, char *result, int count)
{
    i2c.write(ADDRESS << 1, &reg, 1, true);
    i2c.read(ADDRESS << 1, result, count);
}

void write(char reg, char data)
{
    char cmd[2] = { reg, data };
    i2c.write(ADDRESS << 1, cmd, 2, true);
}

void getAcceleration(int16_t *x, int16_t *y, int16_t *z)
{

}

void getGyro(int16_t *x, int16_t *y, int16_t *z)
{

}

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

    // Set gyro range
    #define GYRO_FS_250  0
    #define GYRO_FS_500  1
    #define GYRO_FS_1000 2
    #define GYRO_FS_2000 3
    #define REG_GYRO_CONFIG 0x1B
    data = read(REG_GYRO_CONFIG);
    data |= (GYRO_FS_500 << 3);
    write(REG_GYRO_CONFIG, data);

    // Set accelerometer range
    #define ACCEL_FS_2G  0
    #define ACCEL_FS_4G  1
    #define ACCEL_FS_8G  2
    #define ACCEL_FS_16G 3
    #define REG_ACCEL_CONFIG 0x1C
    data = read(REG_ACCEL_CONFIG);
    data |= (ACCEL_FS_4G << 3);
    write(REG_ACCEL_CONFIG, data);

    wait_ms(1000);

    // Read sensors

    #define REG_ACCEL_XOUT_H 0x3B
    char accel[6];
    read_multi(REG_ACCEL_XOUT_H, accel, 6);

    serial.putc(accel[0]);
    serial.putc(accel[1]);
    serial.putc(accel[2]);
    serial.putc(accel[3]);
    serial.putc(accel[4]);
    serial.putc(accel[5]);

    #define REG_GYRO_XOUT_H  0x43
    char gyro[6];
    read_multi(REG_GYRO_XOUT_H, gyro, 6);

    serial.putc(gyro[0]);
    serial.putc(gyro[1]);
    serial.putc(gyro[2]);
    serial.putc(gyro[3]);
    serial.putc(gyro[4]);
    serial.putc(gyro[5]);

    led_2 = 0;
    led_3 = 1;

    // i2c.start();
    // char cmd[2] = { 0x6B, 0x00 };
    // i2c.write(address, cmd, 2);
    // i2c.stop();
}

#if 0
#include "MPU6050.cpp"
#include "hmc5883l.cpp"

MPU6050 mpu(p28, p27);
HMC5883 hmc(p9, p10);

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
#if 0
    led_1 = 0;
    led_2 = 0;
    led_3 = 0;
    led_4 = 0;

    bool hmc_ok = hmc.init(HMC_FREQUENCY_100KHZ, 
                           HMC_AVERAGING_SAMPLES_1,
                           HMC_DATA_OUTPUT_RATE_75,
                           HMC_BIAS_NONE,
                           HMC_GAIN_1090,
                           HMC_CONTINUOUS_MODE);

    if (!hmc_ok)
    {
        serial.putc(0xbd);
        led_1 = 1;
        return 0;
    }

    bool mpu_ok = mpu.initialize();
    if (!mpu_ok)
    {
        serial.putc(0xbd);
        led_2 = 1;
        return 0;
    }

    // Set the accelerometer scale in multiples of the gravity constant (9.81 m/s^2).
    mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_2);

    // Set the gyro scale in degrees/s
    mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_250);

    wait_ms(100);
    led_3 = 1;
    int handshake = 0xab;
    while (serial.getc() != handshake) { } // Wait until computer says go!
    while (1)
    {
        SensorPacket packet = {};
        hmc.getXYZ(&packet.mag_x, &packet.mag_y, &packet.mag_z);
        mpu.getAcceleration(&packet.accel_x, &packet.accel_y, &packet.accel_z);
        mpu.getRotation(&packet.gyro_x, &packet.gyro_y, &packet.gyro_z);

        for (size_t i = 0; i < sizeof(SensorPacket); i++)
            serial.putc(packet.raw_bytes[i]);

        while (serial.getc() != handshake)
        {
            wait_ms(1);
        }
        wait_ms(16);
    }
#elif 0

    led_3 = 1;

    bool mpu_ok = mpu.initialize();
    if (!mpu_ok)
    {
        led_2 = 1;
        serial.putc(0xbd);
        wait_ms(1000);
        return 0;
    }

    // Set the accelerometer scale in multiples of the gravity constant (9.81 m/s^2).
    mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_2);

    // Set the gyro scale in degrees/s
    mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_250);

    wait_ms(1000);

    while (1) {
        led_1 = led_1 == 1 ? 0 : 1;

        SensorPacket packet = {};
        mpu.getAcceleration(&packet.accel_x, &packet.accel_y, &packet.accel_z);
        mpu.getRotation(&packet.gyro_x, &packet.gyro_y, &packet.gyro_z);
        for (size_t i = 0; i < sizeof(SensorPacket); i++)
            serial.putc(packet.raw_bytes[i]);

        const int HANDSHAKE = 0xab;
        while (serial.getc() != HANDSHAKE)
        {
            wait_ms(100);
        }
    }
#endif

    return 0;
}
#endif