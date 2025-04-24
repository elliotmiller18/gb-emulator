#include "memory.h"
#include "register.h"

Memory::Memory(Registers& registers) : registers(registers) {}

Memory::~Memory() {}

uint8_t Memory::read(uint16_t addr) {
    return memory[addr];
}

uint8_t Memory::read(Register16 reg) {
    return memory[registers.read(reg)];
}

//TODO: WRITING FROM 16 BIT REGISTERS

bool Memory::write(uint16_t addr, uint8_t value) {
    return __write(addr, value);
}

bool Memory::write(uint16_t addr, Register8 value) {
    return __write(addr, registers.read_half(value));
}

bool Memory::write(Register16 addr, uint8_t value) {
    return __write(registers.read(addr), value);
}

bool Memory::write(Register16 addr, Register8 value) {
    return __write(registers.read(addr), registers.read_half(value));
}

bool Memory::__write(uint16_t addr, uint8_t val) {}
