// g++ ./sensor_serial.cpp -o sensor_serial

#include <iostream>
#include <stdint.h>
#include <cstdio>
#include "serial.h"
#include "serial.cpp"

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;

int main(int argc, char **argv)
{
    assert(argc == 3);
    const char *port_name = argv[1]; // mbed shows up as ttyACM0 on my machine
    int baud_rate = atoi(argv[2]); // 9600 is usually good, they say?
    Serial port = serial_open(port_name, baud_rate);
    assert(port.is_valid);

    struct SensorPacket
    {
        union
        {
            uint8 raw_bytes[18];
            struct
            {
                int16 mag_x, mag_y, mag_z;
                int16 gyro_x, gyro_y, gyro_z;
                int16 accel_x, accel_y, accel_z;
            };
        };
    };

    int n = 1000;
    const int BUFFER_LENGTH = sizeof(SensorPacket) + 1;
    uint8 buffer[BUFFER_LENGTH];

    actual_serial_flush(&port);

    // Synchronize
    uint8 handshake = 0xab;
    serial_write(&port, &handshake, 1);

    while (n > 0)
    {
        n--;
        for (int i = 0; i < BUFFER_LENGTH; i++)
            buffer[i] = 0;
        buffer[BUFFER_LENGTH - 1] = '\0';

        while (serial_bytes_available(&port) < sizeof(SensorPacket))
        {
            // Wait
        }

        while (serial_bytes_available(&port) >= sizeof(SensorPacket))
        {
            int bytes_read = serial_read(&port, buffer, sizeof(SensorPacket));

            SensorPacket *read_packet = (SensorPacket*)buffer;
            printf("%d\t%d\t%d\t", read_packet->mag_x, read_packet->mag_y, read_packet->mag_z);
            printf("%d\t%d\t%d\t", read_packet->gyro_x, read_packet->gyro_y, read_packet->gyro_z);
            printf("%d\t%d\t%d\n", read_packet->accel_x, read_packet->accel_y, read_packet->accel_z);
        }

        serial_write(&port, &handshake, 1);
    }

    serial_close(&port);

    return 0;
}
