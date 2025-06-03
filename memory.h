#ifndef MEMORY_H
#define MEMORY_H

#include <array>
#include "register.h"

constexpr int MEMORY_SIZE = 0xFFFF;
constexpr int EIGHT_BIT_MEM_PREFIX = 0xFF00;
constexpr bool BYTE_ADDRESSABLE = true;
constexpr bool GB_CPU_LITTLE_ENDIAN = true;
constexpr bool GB_CPU_BIG_ENDIAN = !GB_CPU_LITTLE_ENDIAN;

class Memory
{
private:
    std::array<uint8_t, MEMORY_SIZE> memory;
    Registers& registers;
public:
    Memory(Registers& registers);
    ~Memory();
    uint16_t unpack_addr(BinOpt addr);
    uint16_t read_word(BinOpt addr);
    uint8_t read_byte(BinOpt addr);
    bool write_word(BinOpt addr, BinOpt16 val);
    bool write_byte(BinOpt addr, BinOpt8 byte);
};

#endif

