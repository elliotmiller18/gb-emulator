#pragma once

#include <array>
#include "register.h"

constexpr int MEMORY_SIZE = 0xFFFF;
constexpr int EIGHT_BIT_MEM_PREFIX = 0xFF00;
constexpr bool BYTE_ADDRESSABLE = true;

class Memory
{
public:
    std::array<uint8_t, MEMORY_SIZE> memory;
    Registers& registers;
    Memory(Registers& registers, const char* filename);
    Memory(Registers& registers) : registers(registers) {}
    ~Memory();
    uint16_t unpack_addr(BinOpt addr);
    uint16_t read_word(BinOpt addr);
    uint16_t read_word_and_inc_sp();
    uint8_t read_byte(BinOpt addr);
    void write_word_and_dec_sp(BinOpt16 val);
    void write_word(BinOpt addr, BinOpt16 val);
    void write_byte(BinOpt addr, BinOpt8 byte);
    void adjust(uint16_t addr, uint8_t adjustment);
    bool tick_timer();
};
