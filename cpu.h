#ifndef CPU_H
#define CPU_H

#include "rom.h"
#include "register.h"
#include <array>

// the value that programs expect the SP to be initialized to
constexpr int STACK_STARTING_VALUE = 0xFFFE;
// the pointer to the top of VRAM
constexpr int VRAM_TOP = 0x9FFF;

class Cpu
{
private:
    Registers registers;
    Rom& rom;

public:
    Cpu(Rom& rom);
    ~Cpu();
};

#endif