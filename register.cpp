#include "utils.h"
#include "register.h"
#include <iostream>
#include <variant>

Registers::Registers() {}

Registers::~Registers() {}

uint16_t Registers::unpack_binopt(BinOpt val) {
    // return the result of the lambda passed to std::visit
    return std::visit([this](auto&& v) -> uint16_t {
        // get the raw type of whatever val is holding
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, Register16>) {return registers[v];}
        else if constexpr (std::is_same_v<T, Register8>) {return registers[half_reg_to_reg(v)];}
        // we know it's either a u8 or u16, both of which will be fine to use as a u16
        return v;
    }, val);
}

void Registers::write(RegisterOpt reg, BinOpt val) {
    uint16_t unpacked_value = unpack_binopt(val);
    Register16 target;
    if (target = *std::get_if<Register16>(&reg)) {}
    // else assign target to whatever 16 bit register half corresponds to
    else if (Register8 half = *std::get_if<Register8>(&reg)) {target = half_reg_to_reg(half);} 
    registers[target] = unpacked_value;
}

uint16_t Registers::read(RegisterOpt reg) {
    // if it's a register16 return the 16 bit value at 
    if (Register16 unpacked_reg = *std::get_if<Register16>(&reg)) { return registers[unpacked_reg]; } 
    else if (Register8 unpacked_reg = *std::get_if<Register8>(&reg)) {
        switch(unpacked_reg) {
            // high half registers
            case A:
            case B:
            case D:
            case H:
                return registers[half_reg_to_reg(unpacked_reg)] >> 8;
            // low half registers
            case C:
            case E:
            case L:
                return (registers[half_reg_to_reg(unpacked_reg)] & LOWER_HALF_MASK);
        }
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
