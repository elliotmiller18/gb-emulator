#include "cpu.h"
#include "rom.h"
#include "register.h"
#include <iostream>

Cpu::Cpu(Rom& rom) : rom(rom), registers(Registers()){
    // due to legal issues (providing the boot rom is illegal) we're going to hard code the behavior of the boot rom.
    // this may be a significant source of bugs 

    // initialize SP
    registers.write(SP, STACK_STARTING_VALUE);
    // this is XOR A in the actual boot rom
    registers.write_half(A, 0);
}

Cpu::~Cpu() {}