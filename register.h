#ifndef REGISTER_H
#define REGISTER_H

#include <variant>
#include <array>

constexpr int NUM_REGISTERS = 6;

enum Register16 {
    AF,
    BC,
    DE,
    HL,
    SP,
    PC,
};

enum Register8 {
    A,
    B,
    C,
    D,
    E,
    H,
    L,
};

enum Flag {
    z,
    n,
    h,
    c,
};

/// A variant between Register16 and Register8.
using RegisterOpt = std::variant<Register16, Register8>;
/// Any 16 bit argument, typically used for addressing.
using BinOpt16 = std::variant<Register16, uint16_t>;
// Any 8 bit argument
using BinOpt8 = std::variant<Register8, uint8_t>;
/// Any binary argument, so a 16/8 bit value or a full/half register.
using BinOpt = std::variant<BinOpt16, BinOpt8>;

class Registers {
private:
    std::array<uint16_t, NUM_REGISTERS> registers;
public:
    Registers();
    ~Registers();
    uint16_t unpack_binopt(BinOpt val);
    uint8_t unpack_binopt8(BinOpt8 val);
    uint16_t unpack_binopt16(BinOpt16 val);
    /// @return the value written to reg
    uint16_t write(Register16 reg, BinOpt16 val);
    void write_half(Register8 reg, BinOpt8 val);
    uint16_t read(Register16 reg);
    uint8_t read_half(Register8 reg);
    /// Returns the full register that a half register is a part of.
    Register16 half_reg_to_reg(Register8 reg);
    void set_flag(Flag flag, bool value);
    bool get_flag(Flag flag);
};

#endif
