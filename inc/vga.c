#include "vga.h"
#include "io.h"

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

