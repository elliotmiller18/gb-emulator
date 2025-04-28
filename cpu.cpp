#include "cpu.h"
#include "rom.h"
#include "register.h"
#include <iostream>

/// @brief Sets the internal state of the cpu like the boot rom.
/// @returns Boot success.
bool Cpu::boot() {
    // due to legal issues (providing the boot rom is illegal) we're going to hard code the behavior of the boot rom.
    // I'm using this as a source: https://realboyemulator.wordpress.com/2013/01/03/a-look-at-the-game-boy-bootstrap-let-the-fun-begin/
    
    // initialize the stack
    registers.write(SP, STACK_STARTING_VALUE);
    // zero out accumulator
    excl_or(A);

    
    
    return false;
}

uint16_t Cpu::excl_or(BinOpt8 arg) {
    uint8_t unpacked = registers.unpack_binopt8(arg);
    unpacked = unpacked xor registers.read_half(A); 
    registers.set_flag(z, unpacked == 0);
    registers.write_half(A, unpacked);
    return unpacked;
}

uint16_t Cpu::load_to_r16(RegisterOpt reg, uint16_t imm) {}

uint16_t Cpu::load_to_mem(RegisterOpt reg, Register16 addr) {}

uint16_t Cpu::load_to_a(Register16 addr) {}

uint16_t Cpu::load_to_addr(uint16_t addr) {}