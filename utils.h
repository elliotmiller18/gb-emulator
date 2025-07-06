#pragma once
#include "register.h"
#include <cstdint>

/// keeps the upper 8 bits
constexpr int UPPER_HALF_MASK = 0xFF00;
/// keeps the lower 8 bits
constexpr int LOWER_HALF_MASK = 0x00FF;

int msb(uint16_t arg);
int lsb(uint16_t arg);
int get_bits_from_range(int target, int start, int end);
uint16_t combine_bytes(uint8_t msb, uint8_t lsb);
Register16 get_register16_from_opcode_bits(int bits);
bool get_bit(int target, int position);
