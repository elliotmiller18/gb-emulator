#include "utils.h"
#include "register.h"
#include <iostream>

Registers::Registers() {}

Registers::~Registers() {}

void Registers::write(Register16 reg, uint16_t val) {
    registers[reg] = val;
}

void Registers::write_half(Register8 reg, uint8_t val) {
    Register16 full = half_reg_to_reg(reg);
    switch(reg) {
        // high half registers
        case A:
        case B:
        case D:
        case H:
            registers[full] &= LOWER_HALF_MASK;
            registers[full] |= (static_cast<uint16_t>(val) << 8);
            break;
        // low half registers
        case C:
        case E:
        case L:
            registers[full] &= UPPER_HALF_MASK;
            registers[full] |= val;
            break;
        default:
            std::cerr << "Unrecognized half register, could not write " << val << std::endl; 
            exit(1);
    }
}

uint16_t Registers::read(Register16 reg) {
    return registers[reg];
}

uint8_t Registers::read_half(Register8 reg) {
    Register16 full = half_reg_to_reg(reg);
    switch(reg) {
        // high half registers
        case A:
        case B:
        case D:
        case H:
            return (registers[full] & UPPER_HALF_MASK) >> 8;
        // half registers
        case C:
        case E:
        case L:
            return registers[full] & LOWER_HALF_MASK;
        default:
            std::cerr << "Unrecognized half register, could not read." << std::endl; 
            exit(1);
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
        default:
            std::cerr << "Unrecognized half register.";
            exit(1);
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
