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
public:
    Memory(Registers& registers);
    ~Memory();
    uint16_t read(BinOpt16 addr);
    bool write(BinOpt16 addr, BinOpt16 val);
    bool write_byte(BinOpt16 addr, BinOpt8 byte);
};

#endif

