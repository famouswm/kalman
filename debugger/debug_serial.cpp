// g++ ./debug_serial -o debug_serial

#include <iostream>
#include <stdint.h>
#include <cstdio>
#include <sys/ioctl.h>
#include <getopt.h>
#include <dirent.h>
#include <termios.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <algorithm>
#include <assert.h>

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;

struct Serial
{
    bool is_valid;
    int handle;
    struct termios options;
};

void serial_close(Serial *serial)
{
    tcsetattr(serial->handle, TCSANOW, &serial->options);
    close(serial->handle);
}

Serial serial_open(const char *port_name, int baud_rate)
{
    Serial result = {};
    result.handle = open(port_name, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (result.handle == -1)
    {
        printf("Failed to open %s\n", port_name);
        printf("Try enabling sudo!\n");
        return result;
    }

    tcgetattr(result.handle, &result.options);

    switch (baud_rate)
    {
        default: // TODO: Actually handle other baud rates!
        {
            cfsetispeed(&result.options, B9600);
            cfsetospeed(&result.options, B9600);
        } break;
    }

    result.options.c_cflag |= (CLOCAL | CREAD);
    result.options.c_cflag &= ~PARENB;
    result.options.c_cflag &= ~CSTOPB;
    result.options.c_cflag &= ~CSIZE;
    result.options.c_cflag |= CS8;
    tcsetattr(result.handle, TCSANOW, &result.options);
    result.is_valid = true;
    return result;
}

// Returns number of bytes written
int serial_write(Serial *port, uint8 *buffer, int length)
{
    assert(port->is_valid);
    int bytes_written = write(port->handle, buffer, length);
    return bytes_written;
}

// Attempt to read <length> bytes into <buffer>
int serial_read(Serial *port, uint8 *buffer, int length)
{
    assert(port->is_valid);
    int bytes_read = read(port->handle, buffer, length);
    return bytes_read;
}

// Clears out the serial port data?
// Don't really know how this works...
void serial_flush(Serial *port)
{
    tcflush(port->handle, TCIOFLUSH);
    tcdrain(port->handle);
}

int serial_bytes_available(Serial *port)
{
    int bytes_available;
    ioctl(port->handle, FIONREAD, &bytes_available);
    return bytes_available;
}

int main(int argc, char **argv)
{
    assert(argc == 3);
    const char *port_name = argv[1]; // mbed shows up as ttyACM0 on my machine
    int baud_rate = atoi(argv[2]); // 9600 is usually good, they say?
    Serial port = serial_open(port_name, baud_rate);
    assert(port.is_valid);

    serial_flush(&port);

    // Debugging routine:
    // 1) ./pc_serial /dev/ttyACM0 9600
    // 2) Input the number of bytes you wish to read
    // 3) The number of bytes read get printed
    int bytes_to_read;
    scanf("%d", &bytes_to_read);
    #define MAX_BUFFER_LENGTH 1024
    uint8 buffer[MAX_BUFFER_LENGTH];
    while (bytes_to_read > 0)
    {
        if (bytes_to_read > MAX_BUFFER_LENGTH - 2)
            bytes_to_read = MAX_BUFFER_LENGTH - 2;

        // Reset buffer of length bytes_to_read + 1 (one more for null-terminator)
        for (int i = 0; i < bytes_to_read + 1; i++)
            buffer[i] = 0;

        // Try to read the specified number of bytes
        int bytes_read = serial_read(&port, &buffer[0], bytes_to_read);
        buffer[bytes_to_read] = '\0';

        // Print to console, decimal first then hex
        printf("%d:\t%s\n\t", bytes_read, (char*)buffer);
        for (int i = 0; i < bytes_read; i++)
            printf("%x ", buffer[i]);
        printf("\n");

        // User writes the number of bytes to read next
        scanf("%d", &bytes_to_read);
    }

    serial_close(&port);

    return 0;
}
