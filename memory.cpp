#include "memory.h"
#include "register.h"
#include <iostream>
#include <variant>

Memory::Memory(Registers& registers) : registers(registers) {}

Memory::~Memory() {}

uint16_t Memory::unpack_address(BinOpt16 addr) {
    if(Register16 reg = *std::get_if<Register16>(&addr)) {
        return registers.read(reg);
    } else if (uint16_t address = *std::get_if<uint16_t>(&addr)) {
        return address;
    } else {
        std::cerr << "Invalid BinOpt16 in Memory::Read"; 
        exit(1);
    }
}

uint16_t Memory::read(BinOpt16 addr) {
    uint16_t address = unpack_address(addr);
    // Note: we don't need to worry about OOB errors as address is a 16 bit integer and memory has 2^16 entries
    if(address < MEMORY_SIZE - 1) {
        return (memory[address+1] << 8) | memory[address];
    } else {
        // if we're loading the very last value in memory into a half register
        return memory[address];
    }
}

bool Memory::write(BinOpt16 addr, BinOpt value) {
    uint16_t unpacked_addr = unpack_address(addr);

    return std::visit([&](auto&& v) -> uint16_t {
        // get the raw type of whatever val is holding
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, Register16>) {
            // check if we can write this high
            if(unpacked_addr == MEMORY_SIZE) {return false;}
            uint16_t writeval = registers.read(v);
            memory[unpacked_addr] = static_cast<uint8_t>(writeval);
            memory[unpacked_addr + 1] = static_cast<uint8_t>(writeval >> 8);
            return true;
        } else if constexpr (std::is_same_v<T, Register8>) {
            memory[unpacked_addr] = static_cast<uint8_t>(registers.read(v));
            return true;
        } else if constexpr (std::is_same_v<T, uint16_t>) {
            // also check if we can write this high for other 16 bit opt
            if(unpacked_addr == MEMORY_SIZE) {return false;}
            memory[unpacked_addr + 1] = static_cast<uint8_t>(v >> 8);
        }
        // for 16 and 8 bit write the lower bits to the lower value
        memory[unpacked_addr] = static_cast<uint8_t>(v);
        return true;
    }, value);
}
