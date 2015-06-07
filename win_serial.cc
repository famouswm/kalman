#include "serial.h"
#include <assert.h>
#include "windows.h"

struct Port
{
    HANDLE handle;
    bool is_open;
};

static Port _serial_port;

bool serial_open(const char *name)
{
    Port result = {};
    result.is_open = false;
    result.handle = CreateFile(name, GENERIC_READ | GENERIC_WRITE,
                      0, 0, OPEN_EXISTING, 0, 0);
    if (result.handle == INVALID_HANDLE_VALUE)
        return false;

    DCB config = {};
    if (!GetCommState(result.handle, &config))
    {
        CloseHandle(result.handle);
        return false;
    }
    // Set options...
    config.BaudRate = 9600;
    config.StopBits = 0;
    config.Parity = 0;
    config.ByteSize = 8;
    if (!SetCommState(result.handle, &config))
    {
        CloseHandle(result.handle);
        return false;
    }

    // See http://www.codeproject.com/Articles/3061/Creating-a-Serial-communication-on-Win
    COMMTIMEOUTS timing;
    timing.ReadIntervalTimeout = 1;
    timing.ReadTotalTimeoutMultiplier = 0;
    timing.ReadTotalTimeoutConstant = 0;
    timing.WriteTotalTimeoutMultiplier = 3;
    timing.WriteTotalTimeoutConstant = 2;
    if (!SetCommTimeouts(result.handle, &timing))
    {
        CloseHandle(result.handle);
        return false;
    }

    result.is_open = true;
    _serial_port = result;
    return true;
}

void serial_close()
{
    assert(_serial_port.is_open);
    CloseHandle(_serial_port.handle);
    _serial_port.is_open = false;
}

int serial_write(char *data, int length)
{
    assert(_serial_port.is_open);
    DWORD bytes_written = 0;
    if (!WriteFile(_serial_port.handle, data, length, &bytes_written, 0))
        return 0;
    return (int)bytes_written;
}

// Can block when not using timeouts
int serial_read(char *data, int length)
{
    assert(_serial_port.is_open);
    DWORD bytes_read = 0;
    if (!ReadFile(_serial_port.handle, data, length, &bytes_read, 0))
        return 0;
    return (int)bytes_read;
}

// TODO: Don't think this actually works
int serial_available()
{
    assert(_serial_port.is_open);
    DWORD bytes_available = 0;
    if (!PeekNamedPipe(_serial_port.handle, 0, 0, 0, &bytes_available, 0))
        return 0;
    return (int)bytes_available;
}

void serial_actual_flush()
{
    assert(_serial_port.is_open);
    DWORD bytes_available = 0;
    if (PeekNamedPipe(_serial_port.handle, 0, 0, 0, &bytes_available, 0))
    {
        ReadFile(_serial_port.handle, 0, bytes_available, 0, 0);
    }
}
