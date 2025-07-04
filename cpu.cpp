#include "cpu.h"
#include "register.h"
#include <iostream>

void Cpu::cycle(int cycles) {
    std::cout << "cycling\n";    
}

int Cpu::fetch_and_inc() {
    uint16_t old_pc = registers.read(PC);
    registers.write(PC, static_cast<uint16_t>(old_pc+1));
    return memory.read_byte(old_pc);
}