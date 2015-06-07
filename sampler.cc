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
    char *port_name   = "COM3";
    int sleep_time    = 16;
    char *output_name = "./matlab/out.txt";
    int sample_count  = 128;
    if (argc == 5)
    {
        port_name = argv[1];
        sleep_time = atoi(argv[2]);
        output_name = argv[3];
        sample_count = atoi(argv[4]);
    }
    else
    {
        printf("Usage: sampler.exe <port> <sleep> <out> <n> where\n");
        printf("<port>: Port name (i.e. COM3)\n");
        printf("<sleep>: Sampling interval in integer milliseconds\n");
        printf("<out>: Output name\n");
        printf("<n>: Number of samples to take\n");
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

        float t = samples * (float)(sleep_time) / 1000.0f;
        fprintf(output, "%f\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n", t,
                packet.mag_x, packet.mag_y, packet.mag_z,
                packet.gyro_x, packet.gyro_y, packet.gyro_z,
                packet.accel_x, packet.accel_y, packet.accel_z);

        samples++;
        Sleep(sleep_time);
    }

    fclose(output);
    serial_close();
    return 0;
}
