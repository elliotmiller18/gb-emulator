#ifndef MEMORY_H
#define MEMORY_H

#include <array>
#include "register.h"

constexpr int MEMORY_SIZE = 0xFFFF;

class Memory
{
private:
    std::array<uint8_t, MEMORY_SIZE> memory;
    Registers& registers;
    uint16_t unpack_address(BinOpt16 addr);
public:
    Memory(Registers& registers);
    ~Memory();
    uint16_t read(BinOpt16 addr);
    bool write(BinOpt16 addr, BinOpt value);
};

#endif

