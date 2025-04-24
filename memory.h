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
    bool __write(uint16_t addr, uint8_t value);
public:
    Memory(Registers& registers);
    ~Memory();
    uint8_t read(uint16_t addr);
    uint8_t read(Register16 reg);
    bool write(uint16_t addr, uint8_t value);
    bool write(uint16_t addr, Register8 reg);
    bool write(Register16 addr, uint8_t value);
    bool write(Register16 addr, Register8 reg);
};

#endif

