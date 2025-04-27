#ifndef CPU_H
#define CPU_H

#include "rom.h"
#include "register.h"
#include <array>

/// the value that programs expect the SP to be initialized to
constexpr uint16_t STACK_STARTING_VALUE = 0xFFFE;
/// the pointer to the top of VRAM
constexpr uint16_t VRAM_TOP = 0x9FFF;

class Cpu
{
private:
    Registers registers;
    Rom& rom;
    bool boot();
    uint16_t excl_or(BinOpt8 arg);
public:
    Cpu(Rom& rom);
    ~Cpu();
};

#endif