#ifndef CPU_H
#define CPU_H

#include "rom.h"
#include "register.h"
#include "memory.h"
#include <array>
#include <variant>

/// the value that programs expect the SP to be initialized to
constexpr uint16_t STACK_STARTING_VALUE = 0xFFFE;
/// the pointer to the top of VRAM
constexpr uint16_t VRAM_TOP = 0x9FFF;

class Cpu
{
private:
    Registers registers;
    Rom& rom;
    Memory mem;
    bool boot();
    uint16_t excl_or(BinOpt8 arg);
    uint16_t load_to_r16(RegisterOpt reg, uint16_t imm);
    uint16_t load_to_mem(RegisterOpt reg, Register16 addr);
    /// loads to a from the address in reg
    uint16_t load_to_a(Register16 addr);
    /// writes the stack pointer to the given address
    uint16_t load_to_addr(uint16_t addr);
public:
    Cpu(Rom& rom) : registers(Registers()), rom(rom), mem(registers) {}
    ~Cpu() {}
};

#endif
