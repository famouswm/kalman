#include "mbed.h"
#include "mpu.cpp"
#include "hmc.cpp"

HMC5883 hmc(p9, p10);
DigitalOut led_1(LED1);
DigitalOut led_2(LED2);
DigitalOut led_3(LED3);
DigitalOut led_4(LED4);
RawSerial serial(USBTX, USBRX);

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
    bool mpu_ok = mpu_init(MPU_GYRO_FS_250, MPU_ACCEL_FS_2G);
    bool hmc_ok = hmc.init(HMC_FREQUENCY_100KHZ,
                           HMC_AVERAGING_SAMPLES_1,
                           HMC_DATA_OUTPUT_RATE_75,
                           HMC_BIAS_NONE,
                           HMC_GAIN_1090,
                           HMC_CONTINUOUS_MODE);
    if (!mpu_ok || !hmc_ok)
    {
        led_3 = 1;
        wait_ms(1000);
        led_1 = 0;
        led_2 = 0;
        led_3 = 0;
        return 0;
    }
    led_2 = 1;
    int handshake = 0xab;
    while (serial.getc() != handshake)
        wait_ms(100);
    led_3 = 1;
    while (1)
    {
        SensorPacket packet = {};
        hmc.getXYZ(&packet.mag_x, &packet.mag_y, &packet.mag_z);
        mpu_read_acceleration(&packet.accel_x, &packet.accel_y, &packet.accel_z);
        mpu_read_gyro(&packet.gyro_x, &packet.gyro_y, &packet.gyro_z);
        for (size_t i = 0; i < sizeof(SensorPacket); i++)
            serial.putc(packet.raw_bytes[i]);
        while (serial.getc() != handshake)
            wait_ms(4);
    }

    led_1 = 0;
    led_2 = 0;
    led_3 = 0;
}
