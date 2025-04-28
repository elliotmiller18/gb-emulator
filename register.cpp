#include "utils.h"
#include "register.h"
#include <iostream>
#include <variant>
#include <cassert>

Registers::Registers() {}

Registers::~Registers() {}

uint16_t Registers::unpack_binopt(BinOpt val) {
    // first unpack val into either a BinOpt16 or BinOpt8
    return std::visit([this](auto&& inner) -> uint16_t {
        using T = decltype(inner);
        if constexpr (std::is_same_v<T, BinOpt16>)     return unpack_binopt16(inner);
        else if constexpr (std::is_same_v<T, BinOpt8>) return static_cast<uint16_t>(unpack_binopt8(inner));
        else {
            std::cerr << "Unrecognized binary option type";
            exit(1);
        }
    }, val);
}

uint8_t Registers::unpack_binopt16(BinOpt16 val) {
    if(Register16* reg = std::get_if<Register16>(&val))  return registers[*reg]; 
    else if(uint16_t* imm = std::get_if<uint16_t>(&val)) return *imm;
    else {
        std::cerr << "Invalid BinOpt16 arg"; 
        exit(0);
    }   
}

uint8_t Registers::unpack_binopt8(BinOpt8 val) {
    if(Register8* reg = std::get_if<Register8>(&val))  return static_cast<uint8_t>(registers[half_reg_to_reg(*reg)]); 
    else if(uint8_t* imm = std::get_if<uint8_t>(&val)) return *imm;
    else {
        std::cerr << "Invalid BinOpt8 arg"; 
        exit(0);
    }                                              
}

void Registers::write(Register16 reg, BinOpt16 val) {
    uint16_t unpacked_value = unpack_binopt16(val);
    registers[reg] = unpacked_value;
}

void Registers::write_half(Register8 reg, BinOpt8 val) {
    uint8_t unpacked_value = unpack_binopt8(val);
    switch(reg) {
        // high half registers
        case A:
        case B:
        case D:
        case H:
            registers[half_reg_to_reg(reg)] &= LOWER_HALF_MASK;
            registers[half_reg_to_reg(reg)] |= (static_cast<uint16_t>(unpacked_value) << 8);
        // low half registers
        case C:
        case E:
        case L:
            registers[half_reg_to_reg(reg)] &= UPPER_HALF_MASK;
            registers[half_reg_to_reg(reg)] |= unpacked_value;
    }
}

uint16_t Registers::read(Register16 reg) {
    return registers[reg];
}

uint8_t Registers::read_half(Register8 reg) {
    switch(reg) {
        // high half registers
        case A:
        case B:
        case D:
        case H:
            return static_cast<uint8_t>(registers[half_reg_to_reg(reg)] >> 8);
        // low half registers
        case C:
        case E:
        case L:
            return static_cast<uint8_t>(registers[half_reg_to_reg(reg)]);
    }
}

Register16 Registers::half_reg_to_reg(Register8 reg) {
    switch(reg) {
        case A:
            return AF;
        case B:
        case C:
            return BC;
        case D:
        case E:
            return DE;
        case H:
        case L:
            return HL;
    }
}

void Registers::set_flag(Flag flag, bool value) {
    int bit = value ? 1 : 0;
    switch(flag) {
        case z: 
            registers[AF] |= bit << 7;
            break;
        case n:
            registers[AF] |= bit << 6;
            break;
        case h:
            registers[AF] |= bit << 5;
            break;
        case c:
            registers[AF] |= bit << 4;
            break;
        default:
            std::cerr << "Unrecognized flag.";
            exit(1);
    } 
}

bool Registers::get_flag(Flag flag) {
    switch(flag) {
        case z: 
            return registers[AF] & (1 << 7);
            break;
        case n:
            return registers[AF] & (1 << 6);
            break;
        case h:
            return registers[AF] & (1 << 5);
            break;
        case c:
            return registers[AF] & (1 << 4);
            break;
        default:
            std::cerr << "Unrecognized flag.";
            exit(1);
    } 
}
