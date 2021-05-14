#include <stdbool.h>

#include "inc/vga.h"
#include "inc/serial.h"

// Strings
char* msg = "Provide input through COM1\nPress BACKSPACE to clear\n\n\0";
char* err = "Error encountered while initializing COM1\n\n";


/* MAIN */

void main(void)
{
    teletype(0x08);                                         // Clear
    outString(msg);                                         // Print message

    if (serial_init(COM1) != 0) outString(err);             // Try initializing serial port

    else while (true) teletype(serial_get(COM1));           // If successful, enter loop
}
