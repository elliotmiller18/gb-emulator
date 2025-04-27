#include "cpu.h"
#include "rom.h"
#include "register.h"
#include <iostream>

Cpu::Cpu(Rom& rom) : rom(rom) {}

Cpu::~Cpu() {}

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
    uint16_t unpacked = registers.unpack_binopt(arg);
    unpacked = unpacked xor registers.read(A); 
    registers.set_flag(z, unpacked == 0);
    registers.write(A, unpacked);
    return unpacked;
}


