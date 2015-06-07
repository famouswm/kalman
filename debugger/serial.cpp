#include "serial.h"
#include <cstdio>
#include <sys/ioctl.h>
#include <getopt.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <algorithm>
#include <assert.h>

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

void actual_serial_flush(Serial *port)
{
    uint8 dump[1024];
    while (serial_bytes_available(port))
        serial_read(port, dump, 1024);
}

int serial_bytes_available(Serial *port)
{
    int bytes_available;
    ioctl(port->handle, FIONREAD, &bytes_available);
    return bytes_available;
}