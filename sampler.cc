#include <stdio.h>
#include "serial.h"
#include "win_serial.cc"
#include <stdint.h>

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

int main(int argc, char **argv)
{
    char *port_name     = "COM3";
    int sleep_time      = 16;
    char *output_name   = "./matlab/out.txt";
    int sample_count    = 128;
    int average_count   = 32;
    if (argc == 6)
    {
        port_name = argv[1];
        sleep_time = atoi(argv[2]);
        output_name = argv[3];
        sample_count = atoi(argv[4]);
        average_count = atoi(argv[5]);
    }
    else
    {
        printf("Usage: sampler.exe <port> <sleep> <out> <n> <m> where\n");
        printf("<port>: Port name (i.e. COM3)\n");
        printf("<sleep>: Sampling interval in integer milliseconds\n");
        printf("<out>: Output name\n");
        printf("<n>: Number of samples to take\n");
        printf("<m>: Number of averaging samples to take\n");
        printf("Using defaults\n\n");
    }

    if (!serial_open(port_name))
    {
        printf("Failed to open port %s\n", port_name);
        return -1;
    }

    FILE *output = fopen(output_name, "w+");

    int index = 0;
    int samples = 0;
    int average_samples = 0;

    int32_t bias_mag_x = 0;
    int32_t bias_mag_y = 0;
    int32_t bias_mag_z = 0;
    int32_t bias_gyro_x = 0;
    int32_t bias_gyro_y = 0;
    int32_t bias_gyro_z = 0;
    int32_t bias_accel_x = 0;
    int32_t bias_accel_y = 0;
    int32_t bias_accel_z = 0;
    while (samples < sample_count)
    {
        char handshake = 0xab;
        serial_write(&handshake, 1);

        SensorPacket packet = {};
        do
        {
            char *ptr = (char*)(packet.raw_bytes + index);
            int length = sizeof(SensorPacket) - index;
            index += serial_read(ptr, length);
            assert(index <= sizeof(SensorPacket));
            if (index == sizeof(SensorPacket))
                index = 0;
        } while (index != 0);

        printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
               packet.mag_x, packet.mag_y, packet.mag_z,
               packet.gyro_x, packet.gyro_y, packet.gyro_z,
               packet.accel_x, packet.accel_y, packet.accel_z);

        if (average_samples < average_count)
        {
            bias_mag_x += packet.mag_x;
            bias_mag_y += packet.mag_y;
            bias_mag_z += packet.mag_z;
            bias_gyro_x += packet.gyro_x;
            bias_gyro_y += packet.gyro_y;
            bias_gyro_z += packet.gyro_z;
            bias_accel_x += packet.accel_x;
            bias_accel_y += packet.accel_y;
            bias_accel_z += packet.accel_z;
            average_samples++;
            if (average_samples == average_count)
            {
                bias_mag_x /= average_count;
                bias_mag_y /= average_count;
                bias_mag_z /= average_count;
                bias_gyro_x /= average_count;
                bias_gyro_y /= average_count;
                bias_gyro_z /= average_count;
                bias_accel_x /= average_count;
                bias_accel_y /= average_count;
                bias_accel_z /= average_count;
                fprintf(output, "%f\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
                        0.0f,
                        bias_mag_x, bias_mag_y, bias_mag_z,
                        bias_gyro_x, bias_gyro_y, bias_gyro_z,
                        bias_accel_x, bias_accel_y, bias_accel_z);
            }
        }
        else
        {
            float t = samples * (float)(sleep_time) / 1000.0f;
            fprintf(output, "%f\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n", t,
                    packet.mag_x, packet.mag_y, packet.mag_z,
                    packet.gyro_x, packet.gyro_y, packet.gyro_z,
                    packet.accel_x, packet.accel_y, packet.accel_z);
            samples++;
        }

        Sleep(sleep_time);
    }

    printf("mag bias:\t%d\t%d\t%d\n",
           bias_mag_x, bias_mag_y, bias_mag_z);
    printf("gyro bias:\t%d\t%d\t%d\n",
           bias_gyro_x, bias_gyro_y, bias_gyro_z);
    printf("accel bias:\t%d\t%d\t%d\n",
           bias_accel_x, bias_accel_y, bias_accel_z);

    fclose(output);
    serial_close();
    return 0;
}
