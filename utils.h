#pragma once
#include <cstdint>

/// keeps the upper 8 bits
constexpr int UPPER_HALF_MASK = 0xFF00;
/// keeps the lower 8 bits
constexpr int LOWER_HALF_MASK = 0x00FF;

int msb(uint16_t arg);
int lsb(uint16_t arg);
