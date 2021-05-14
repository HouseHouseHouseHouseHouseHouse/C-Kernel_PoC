#include <stdint.h>
#include <stddef.h>

#ifndef VGA_H
#define VGA_H

// Addressing system
static uint16_t* buffer;
static size_t offset;

// Total number of characters onscreen
static const size_t entries = 25 * 80;

// VGA TTY Functions
void clear(void);
void teletype(char);
void outString(char*);

#endif
