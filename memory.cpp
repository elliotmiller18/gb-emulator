#include "memory.h"
#include "register.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <variant>

Memory::Memory(Registers& registers, const char* filename)
  : registers(registers)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        throw std::runtime_error(std::string("Failed to open ROM: ") + filename);
    }

    auto size = file.tellg();
    if (size < 0 || static_cast<std::size_t>(size) > memory.size()) {
        throw std::runtime_error("ROM size is invalid or too large for memory");
    }

    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(memory.data()), size);
    if (!file) {
        throw std::runtime_error("Error while reading ROM into memory");
    }
}

Memory::~Memory() {}

uint16_t Memory::unpack_addr(BinOpt addr) {
    if(BinOpt16* unpacked = std::get_if<BinOpt16>(&addr)) {
        return registers.unpack_binopt16(*unpacked);
    } else {
        return static_cast<uint16_t>(registers.unpack_binopt8(std::get<BinOpt8>(addr))) | EIGHT_BIT_MEM_PREFIX;
    }
}

uint16_t Memory::read_word_and_inc_sp() {
    uint16_t data = read_word(registers.read(SP));
    registers.write(SP, static_cast<uint16_t>(registers.read(SP) + 2));
    return data;
}

uint16_t Memory::read_word(BinOpt addr) {
    // TODO: validate implementation
    uint16_t unpacked_addr = unpack_addr(addr);

    uint16_t lower_byte = read_byte(unpacked_addr);
    uint16_t upper_byte = read_byte(++unpacked_addr);

    return (upper_byte << 8) | lower_byte;
}

uint8_t Memory::read_byte(BinOpt addr) {
    uint16_t address = unpack_addr(addr);
    // std::cout << "\nReading byte " << std::hex << static_cast<int>(memory[address]) << " from addr " << std::hex << static_cast<int>(address) << "\n";
    return memory[address];
}

bool Memory::write_word(BinOpt addr, BinOpt16 val) {
    // TODO: validate implementation
    uint16_t unpacked_value = registers.unpack_binopt16(val);
    uint16_t unpacked_addr = unpack_addr(addr);

    uint8_t upper_bits = msb_16(unpacked_value);
    uint8_t lower_bits = lsb_16(unpacked_value);

    write_byte(unpacked_addr, lower_bits);
    write_byte(++unpacked_addr, upper_bits);
    return true;
}

/// decrements, then writes
void Memory::write_word_and_dec_sp(BinOpt16 val) {
    registers.write(SP, static_cast<uint16_t>(registers.read(SP) - 2));
    write_word(registers.read(SP), val);
}

bool Memory::write_byte(BinOpt addr, BinOpt8 val) {
    uint8_t unpacked_value = registers.unpack_binopt8(val);
    uint16_t unpacked_addr = unpack_addr(addr);
    // std::cout << "Writing byte " << std::hex << static_cast<int>(unpacked_value) << " to addr " << std::hex << static_cast<int>(unpacked_addr) << "\n";
    memory[unpacked_addr] = unpacked_value;
    return true;
}