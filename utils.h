#pragma once
#include "register.h"
#include <cstdint>

/// keeps the upper 8 bits
constexpr int UPPER_HALF_MASK = 0xFF00;
/// keeps the lower 8 bits
constexpr int LOWER_HALF_MASK = 0x00FF;

const int VERTICAL_VAL8_START = 3;
const int VERTICAL_VAL8_END = 5;

int msb_16(uint16_t arg);
int lsb_16(uint16_t arg);
int msb_8(uint8_t arg);
int lsb_8(uint8_t arg);
int get_bits_in_range(unsigned int target, unsigned int start, unsigned int end);
uint16_t combine_bytes(uint8_t msb, uint8_t lsb);
Register16 get_register16_from_opcode_bits(int bits);
bool get_bit(int target, int position);
RegisterOpt get_dest8_from_bits(int bits);
