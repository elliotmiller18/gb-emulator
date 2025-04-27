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
    // this may be a significant source of bugs A
    return false;
}
