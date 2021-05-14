#include <stdint.h>

#ifndef SERIAL_H
#define SERIAL_H

// Serial I/O port numbers
#define COM1 0x03F8
#define COM2 0x02F8

// Serial functions
int serial_init(uint16_t);
char serial_get(uint16_t);

#endif
