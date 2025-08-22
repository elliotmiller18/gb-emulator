#pragma once

#include <array>
#include "register.h"

constexpr int MEMORY_SIZE = 0xFFFF;
constexpr int EIGHT_BIT_MEM_PREFIX = 0xFF00;
constexpr bool BYTE_ADDRESSABLE = true;

class Cpu;
class Memory
{
public:
    //TODO: heap allocate this no clue why i did it on the stack this WILL cause crashes
    std::array<uint8_t, MEMORY_SIZE> memory;
    Registers& registers;
    Cpu& cpu;

    Memory(Registers& registers, const char* filename, Cpu& cpu);
    Memory(Registers& registers, Cpu& cpu) : registers(registers), cpu(cpu) {}
    ~Memory();
    uint16_t unpack_addr(BinOpt addr) const;
    uint16_t read_word(BinOpt addr) const;
    uint16_t read_word_and_inc_sp();
    uint8_t read_byte(BinOpt addr) const;
    void write_word_and_dec_sp(BinOpt16 val);
    void write_word(BinOpt addr, BinOpt16 val);
    void write_byte(BinOpt addr, BinOpt8 byte);
    bool tick_timer();
    void tick_divider();
};
