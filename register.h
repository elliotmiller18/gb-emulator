#ifndef REGISTER_H
#define REGISTER_H

#include <array>
#include <utils.h>

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

class Registers {
private:
    std::array<uint16_t, NUM_REGISTERS> registers;
public:
    Registers();
    ~Registers();
    void write_reg(Register16 reg, uint16_t val);
    void write_half_reg(Register8 reg, uint8_t val);
    uint16_t read_reg(Register16 reg);
    uint8_t read_half_reg(Register8 reg);
    /// Returns the full register that a half register is a part of.
    Register16 half_reg_to_reg(Register8 reg);
    void set_flag(Flag flag, bool value);
    bool get_flag(Flag flag);
};

#endif