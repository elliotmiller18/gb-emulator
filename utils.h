#ifndef UTILS_H
#define UTILS_H

#include <array>

// total size of physical memory in bytes
#define MEMORY_SIZE 0xFFFF
// the starting position of the non boot rom user code
#define USER_CODE_STARTING_ADDR 0x0100
/// keeps the upper 8 bits
#define UPPER_HALF_MASK 0xFF00
/// keeps the lower 8 bits
#define LOWER_HALF_MASK 0x00FF
#define NUM_REGISTERS 6
#endif