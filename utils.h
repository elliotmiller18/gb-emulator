#pragma once
#include "register.h"
#include "io_ports.h"
#include <chrono>


// 2 bits per pixel
using scanline_t = std::bitset<SCREEN_WIDTH * 2>;

//shared constants
/// rate (hz) at which the div register is incremented, this is also 1 << 14 or 2^14
constexpr int MASTER_CLOCK_SPEED_HZ = 4'194'304;
// 1 second (1 billion ns) 
constexpr long long SECOND_NS = 1'000'000'000.0;
// the length of one machine cycle
constexpr double SYSTEM_CLOCK_SPEED_HZ = MASTER_CLOCK_SPEED_HZ / 4;
constexpr auto SYSTEM_CLOCK_TICKRATE_NS = std::chrono::nanoseconds(static_cast<long long>(SECOND_NS / SYSTEM_CLOCK_SPEED_HZ));
/// keeps the upper 8 bits
constexpr int UPPER_HALF_MASK = 0xFF00;
/// keeps the lower 8 bits
constexpr int LOWER_HALF_MASK = 0x00FF;

constexpr int VERTICAL_VAL8_START = 3;
constexpr int VERTICAL_VAL8_END = 5;
constexpr int HORIZONTAL_DEST8_START_BIT = 4;
constexpr int HORIZONTAL_DEST8_END_BIT = 5;

constexpr int SCREEN_WIDTH = 160;
constexpr int SCREEN_HEIGHT = 144;

// the start of not usable memory
constexpr address_t NULL_ADDR = 0xFE00;

//opcodes
constexpr uint16_t RETI_OPCODE = 0xD9;
constexpr uint16_t EI_OPCODE = 0xFB;

//uint8_t so we can use reset_bit
constexpr uint8_t VBLANK_INTERRUPT_CONTROL_BIT = 0;
//lcd and state are the same thing
constexpr uint8_t LCD_INTERRUPT_CONTROL_BIT = 1;
constexpr uint8_t STAT_INTERRUPT_CONTOL_BIT = LCD_INTERRUPT_CONTROL_BIT;
constexpr uint8_t TIMER_INTERRUPT_CONTROL_BIT = 2;
// serial unused for the forseeable future (this is for serial data transfer between gameboys)
constexpr uint8_t SERIAL_INTERRUPT_CONTROL_BIT = 3;
constexpr uint8_t JOYPAD_INTERRUPT_CONTROL_BIT = 4;

int msb_16(uint16_t arg);
int lsb_16(uint16_t arg);
int msb_8(uint8_t arg);
int lsb_8(uint8_t arg);
uint16_t combine_bytes(uint8_t msb, uint8_t lsb);
uint8_t reset_bit(int bit, uint8_t arg);
void reset_bit(int bit, uint8_t* arg);
uint8_t set_bit(int bit, uint8_t arg);
void set_bit(int bit, uint8_t* arg);
uint8_t set_or_reset_bit(int bit, uint8_t arg, bool set);
void set_or_reset_bit(int bit, uint8_t* arg, bool set);
int get_bits_in_range(unsigned int target, unsigned int start, unsigned int end);
bool get_bit(int target, int position);

int arith8_mcycles(int opcode);
int prefixed_ldh_mcycles(int opcode);
Register16 get_register16_from_opcode(int bits);
RegisterOpt get_dest8_from_bits(int bits);
