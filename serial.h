#ifndef _serial_h_
#define _serial_h_

bool serial_open(const char *name);
void serial_close();
int serial_write(char *data, int length);
int serial_read(char *data, int length);
int serial_available();
void serial_actual_flush();

#endif
