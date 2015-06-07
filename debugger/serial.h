#ifndef SERIAL_H
#define SERIAL_H
#include <stdint.h>
#include <termios.h>

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

void serial_close(Serial *serial);
Serial serial_open(const char *port_name, int baud_rate);
int serial_write(Serial *port, uint8 *buffer, int length);
int serial_read(Serial *port, uint8 *buffer, int length);
void actual_serial_flush(Serial *port);
void serial_flush(Serial *port);
int serial_bytes_available(Serial *port);

#endif