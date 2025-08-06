#pragma once
#include "register.h"
#include <cstdint>

//shared constants

/// keeps the upper 8 bits
constexpr int UPPER_HALF_MASK = 0xFF00;
/// keeps the lower 8 bits
constexpr int LOWER_HALF_MASK = 0x00FF;

constexpr int VERTICAL_VAL8_START = 3;
constexpr int VERTICAL_VAL8_END = 5;
constexpr int HORIZONTAL_DEST8_START_BIT = 4;
constexpr int HORIZONTAL_DEST8_END_BIT = 5;

constexpr uint16_t DIV_ADDR = 0xFF04;
constexpr uint16_t TIMER_COUNTER_ADDR = 0xFF05;
constexpr uint16_t TIMER_MODULO_ADDR = 0xFF06;
constexpr uint16_t TIMER_CONTROL_ADDR = 0xFF07;

constexpr uint16_t INTERRUPT_FLAG_ADDR = 0xFF0F;
constexpr uint16_t INTERRUPT_ENABLE_ADDR = 0xFFFF;

//opcodes
constexpr uint16_t RETI_OPCODE = 0xD9;
constexpr uint16_t EI_OPCODE = 0xFB;

//uint8_t so we can use reset_bit
constexpr uint8_t VBLANK_CONTROL_BIT = 0;
constexpr uint8_t LCD_CONTROL_BIT = 1;
constexpr uint8_t TIMER_CONTROL_BIT = 2;
// serial unused for the forseeable future (this is for serial data transfer between gameboys)
constexpr uint8_t SERIAL_CONTROL_BIT = 3;
constexpr uint8_t JOYPAD_CONTROL_BIT = 4;

int msb_16(uint16_t arg);
int lsb_16(uint16_t arg);
int msb_8(uint8_t arg);
int lsb_8(uint8_t arg);
uint8_t reset_bit(int bit, uint8_t arg);
uint8_t set_bit(int bit, uint8_t arg);
uint8_t set_or_reset_bit(int bit, uint8_t arg, bool set);
int get_bits_in_range(unsigned int target, unsigned int start, unsigned int end);

int arith8_mcycles(int opcode);
int prefixed_ldh_mcycles(int opcode);
uint16_t combine_bytes(uint8_t msb, uint8_t lsb);
Register16 get_register16_from_opcode(int bits);
bool get_bit(int target, int position);
RegisterOpt get_dest8_from_bits(int bits);
