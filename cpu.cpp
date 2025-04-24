#include "cpu.h"
#include "rom.h"
#include "register.h"
#include <iostream>

Cpu::Cpu(Rom& rom) : rom(rom), registers(Registers()){}

Cpu::~Cpu() {}