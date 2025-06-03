#include "memory.h"
#include "register.h"
#include <iostream>
#include <variant>

Memory::Memory(Registers& registers) : registers(registers) {}

Memory::~Memory() {}

uint16_t Memory::unpack_addr(BinOpt addr) {
    if(BinOpt16* unpacked = std::get_if<BinOpt16>(&addr)) {
        return registers.unpack_binopt16(*unpacked);
    } else {
        return static_cast<uint16_t>(registers.unpack_binopt8(std::get<BinOpt8>(addr))) | EIGHT_BIT_MEM_PREFIX;
    }
}

uint16_t Memory::read_word(BinOpt addr) {
    // TODO: validate implementation
    uint16_t unpacked_addr = unpack_addr(addr);

    uint16_t upper_byte = memory[unpacked_addr + 1];
    uint16_t lower_byte = memory[unpacked_addr];

    if(GB_CPU_BIG_ENDIAN) {
        uint16_t temp = upper_byte;
        upper_byte = lower_byte;
        lower_byte = temp;
    }

    return (upper_byte << 8) | lower_byte;
}

uint8_t Memory::read_byte(BinOpt addr) {
    uint16_t address = unpack_addr(addr);
    return memory[address];
}

bool Memory::write_word(BinOpt addr, BinOpt16 val) {
    // TODO: validate implementation
    uint16_t unpacked_value = registers.unpack_binopt16(val);
    uint16_t unpacked_addr = unpack_addr(addr);

    uint8_t upper_bits = static_cast<uint8_t>(unpacked_value >> 8);
    uint8_t lower_bits = static_cast<uint8_t>(unpacked_value);

    if(GB_CPU_BIG_ENDIAN) {
        uint8_t temp = upper_bits;
        upper_bits = lower_bits;
        lower_bits = temp;
    }

    memory[unpacked_addr + 1] = upper_bits;
    memory[unpacked_addr] = lower_bits;
    return true;
}

bool Memory::write_byte(BinOpt addr, BinOpt8 val) {
    uint8_t unpacked_value = registers.unpack_binopt8(val);
    uint16_t unpacked_addr = unpack_addr(addr);
    memory[unpacked_addr] = unpacked_value;
    return true;
}