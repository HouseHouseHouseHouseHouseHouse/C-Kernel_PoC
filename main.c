#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Serial I/O port numbers
#define COM1 0x03F8
#define COM2 0x02F8

// Addressing system
uint16_t* buffer;
size_t offset;

// Total number of characters onscreen
const size_t entries = 25 * 80;

// Strings
char* msg = "Provide input through COM1\nPress BACKSPACE to clear\n\n\0";
char* err = "Error encountered while initializing COM1\n\n";


/* I/O FUNCTIONS */

// Output byte to port
static inline void outb(uint16_t port, uint8_t x)
{
    asm volatile ( "outb %0, %1"
        :
        : "a"(x), "Nd"(port)
    );
}

// Get byte of input from port
static inline uint8_t inb(uint16_t port)
{
    uint8_t x;

    asm volatile ( "inb %1, %0"
        : "=a"(x)
        : "Nd"(port)
    );

    return x;
}


/* SERIAL FUNCTIONS */

// Initialize serial port
static inline int serial_init(uint16_t port)
{
    outb(port + 1, 0x00);               // Disable interrupts

    outb(port + 3, 0x80);               // Enable DLAB (set baud)
    outb(port + 0, 0x0C);               // Low byte of divisor
    outb(port + 1, 0x00);               // High byte of divisor     - 9600 baud

    outb(port + 3, 0x03);               // 8-bit chars, 1 stop bit, no parity

    outb(port + 2, 0xC7);               // Enable FIFO
    outb(port + 4, 0x0B);               // Enable IRQs

    outb(port + 4, 0x1E);               // Loopback mode
    outb(port + 0, 0xAA);               // Test serial chip

    // If we don't receive correctly, error
    if (inb(port) != 0xAA) return -1;

    // Otherwise, normal operation
    outb(port + 4, 0x0F);
    return 0;
}

// Retrieve input from a serial port
static inline char serial_get(uint16_t port)
{
    // Wait to receive a byte, then return it
    while ((inb(port + 5) & 0x01) == 0);
    return inb(port);
}


/* VGA TTY FUNCTIONS */

// Clear the screen and reset cursor
void clear(void)
{
    // Clear out entries
    buffer = (uint16_t*) 0xB8000;                           // Reset VGA buffer pointer

    for (size_t entry = 0; entry < entries; entry++) {
        buffer[entry] = 0x0720;                             // Blank character
    }

    offset = 0;                                             // Reset position

    // Enable cursor
    outb(0x03D4, 0x0A);
    outb(0x03D5, (inb(0x03D5) & 0xC0) | 0x00);              // Top scanline

    outb(0x03D5, 0x0B);
    outb(0x03D5, (inb(0x03D5) & 0xE0) | 0x0C);              // Bottom scanline
}

// Handle/output a character to the screen and advance
void teletype(char c)
{
    // Handle character
    switch (c) {

        // Handle backspace
        case '\b':
        case 0x7F:
            clear();
            break;

        // Handle RETURN
        case '\n':
        case '\r':
            offset /= 80;
            offset++;
            offset *= 80;
            break;

        // Handle NULL
        case '\0':
            break;

        // Otherwise, print
        default:
            buffer[offset] = c | 0x07 << 8;                 // Place char with color in VGA buffer
            offset++;                                       // Advance
            break;
    
    }
    
    // Set the cursor
    outb(0x03D4, 0x0F);
    outb(0x03D5, (uint8_t) offset & 0xFF);                  // Lower byte of index

    outb(0x03D4, 0x0E);
    outb(0x03D5, (uint8_t) (offset >> 8) & 0xFF);           // Upper byte of index
}

// Output a string of characters
void outString(char* string)
{
    // For each byte in the string, print
    for (size_t i = 0; string[i] != 0x00; i++) {
        teletype(string[i]);
    }

}


/* MAIN */

void main(void)
{
    teletype(0x08);                                         // Clear
    outString(msg);                                         // Print message

    if (serial_init(COM1) != 0) outString(err);             // Try initializing serial port

    else while (true) teletype(serial_get(COM1));           // If successful, enter loop
}
