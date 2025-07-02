#include "utils.h"
#include "register.h"
#include <variant>
#include <stdexcept>

Registers::Registers() {}

Registers::~Registers() {}

uint16_t Registers::unpack_binopt(BinOpt val) {
    // first unpack val into either a BinOpt16 or BinOpt8
    return std::visit([this](auto&& inner) -> uint16_t {
        using T = decltype(inner);
        if constexpr (std::is_same_v<T, BinOpt16>)     return unpack_binopt16(inner);
        else if constexpr (std::is_same_v<T, BinOpt8>) return static_cast<uint16_t>(unpack_binopt8(inner));
        else throw std::invalid_argument("Invalid BinOpt type");
    }, val);
}

uint16_t Registers::unpack_binopt16(BinOpt16 val) {
    if(Register16* reg = std::get_if<Register16>(&val))  return read(*reg); 
    else if(uint16_t* imm = std::get_if<uint16_t>(&val)) return *imm;
    else throw std::invalid_argument("Invalid BinOpt16 type");
}

uint8_t Registers::unpack_binopt8(BinOpt8 val) {
    if(Register8* reg = std::get_if<Register8>(&val))  return read_half(*reg); 
    else if(uint8_t* imm = std::get_if<uint8_t>(&val)) return *imm;
    else throw std::invalid_argument("Invalid BinOpt8 type");
}

uint16_t Registers::write(Register16 reg, BinOpt16 val) {
    if(static_cast<int>(reg) > NUM_REGISTERS) throw std::invalid_argument("Invalid register16");
    uint16_t unpacked_value = unpack_binopt16(val);
    registers[reg] = unpacked_value;
    return unpacked_value;
}

void Registers::write_half(Register8 reg, BinOpt8 val) {
    uint8_t unpacked_value = unpack_binopt8(val);
    switch(reg) {
        // high half registers
        case A: case B: case D: case H:
            registers[half_reg_to_reg(reg)] &= LOWER_HALF_MASK;
            registers[half_reg_to_reg(reg)] |= (static_cast<uint16_t>(unpacked_value) << 8);
            break;
        // low half registers
        case C: case E: case L:
            registers[half_reg_to_reg(reg)] &= UPPER_HALF_MASK;
            registers[half_reg_to_reg(reg)] |= unpacked_value;
            break;
        default:
            throw std::invalid_argument("Invalid register8"); 
    }
}

uint16_t Registers::read(Register16 reg) {
    if(static_cast<int>(reg) > NUM_REGISTERS) throw std::invalid_argument("Invalid register16");
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
        default:
            throw std::invalid_argument("Invalid register8"); 
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
            throw std::invalid_argument("Invalid register8"); 
    }
}

void Registers::set_flag(Flag flag, bool value) {
    int bit = value ? 1 : 0;
    int shift;
    switch(flag) {
        case z: 
            shift = 7;
            break;
        case n:
            shift = 6;
            break;
        case h:
            shift = 5;
            break;
        case c:
            shift = 4;
            break;
        default:
            throw std::invalid_argument("Invalid flag"); 
    }
    registers[AF] = (registers[AF] & ~(bit << shift)) | (bit << shift);
}

bool Registers::get_flag(Flag flag) {
    switch(flag) {
        case z: 
            return registers[AF] & (1 << 7);
        case n:
            return registers[AF] & (1 << 6);
        case h:
            return registers[AF] & (1 << 5);
        case c:
            return registers[AF] & (1 << 4);
        default:
            throw std::invalid_argument("Invalid flag"); 
    } 
}
