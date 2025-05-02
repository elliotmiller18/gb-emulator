#include "memory.h"
#include "register.h"
#include <iostream>
#include <variant>

Memory::Memory(Registers& registers) : registers(registers) {}

Memory::~Memory() {}

uint16_t Memory::read(BinOpt16 addr) {
    uint16_t address = registers.unpack_binopt16(addr);
    // Note: we don't need to worry about OOB errors as address is a 16 bit integer and memory has 2^16 entries
    if(address < MEMORY_SIZE - 1) {
        return (memory[address+1] << 8) | memory[address];
    } else {
        // if we're loading the very last value in memory into a half register
        return memory[address];
    }
}

uint8_t Memory::read_byte(BinOpt16 addr) {
    uint16_t address = registers.unpack_binopt16(addr);
    return memory[address];
}

bool Memory::write(BinOpt16 addr, BinOpt16 val) {
    uint16_t unpacked_value = registers.unpack_binopt16(val);
    uint16_t unpacked_addr = registers.unpack_binopt16(addr);
    if(unpacked_addr == MEMORY_SIZE) return false;
    memory[unpacked_value] = static_cast<uint8_t>(unpacked_value);
    memory[unpacked_value + 1] = static_cast<uint8_t>(unpacked_value >> 8);
    return true;
}

bool Memory::write_byte(BinOpt16 addr, BinOpt8 val) {
    uint8_t unpacked_value = registers.unpack_binopt8(val);
    uint16_t unpacked_addr = registers.unpack_binopt16(addr);
    memory[unpacked_addr] = unpacked_value;
    return true;
}