#ifndef CPU_H
#define CPU_H

#include "rom.h"
#include "register.h"
#include <array>

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