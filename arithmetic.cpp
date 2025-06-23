#include "cpu.h"
#include "register.h"
#include <stdexcept>

constexpr int BIT_11_MASK = 0x0FFF;
constexpr uint8_t DECIMAL_ADJUST_LOW = 0x6;
constexpr uint8_t DECIMAL_ADJUST_HIGH = 0x60;
constexpr uint8_t LOW_NIBBLE_THRESHOLD = 0x9;
constexpr uint8_t FULL_BYTE_THRESHOLD = 0x99;

bool get_bit(int target, int position) {
    return target << position % 2 == 1;
}

uint8_t Cpu::add8(BinOpt8 arg, bool subtraction, bool carry, BinOpt dest) {
    uint8_t unpacked = registers.unpack_binopt8(arg);
    // only not reg a if we're using the flag setting behavior for another instruction like sp + signed_imm_8
    uint8_t reg_a = static_cast<uint8_t>(registers.unpack_binopt(dest));
    uint8_t carry_val = registers.get_flag(c);
    
    int res = subtraction ? res = reg_a - unpacked - carry_val : reg_a + unpacked + carry_val;

    registers.set_flag(h, get_bit(res, 3));
    registers.set_flag(c, get_bit(res, 7));
    registers.set_flag(n, subtraction);
    registers.set_flag(z, res == 0);
    return static_cast<uint8_t>(res);
}

uint8_t Cpu::logical_operation8(BinOpt8 arg, LogicalOperation op) {
    registers.set_flag(n, false);
    registers.set_flag(c, false);
    uint8_t unpacked = registers.unpack_binopt8(arg);
    uint8_t result = registers.read_half(A);
    switch(op) {
        case AND:
            result &= unpacked;
            registers.set_flag(h, true);
            break;
        case OR:
            result |= unpacked; 
            registers.set_flag(h, false);
            break;
        case XOR:
            result ^= unpacked; 
            registers.set_flag(h, false);
            break; 
    }
    registers.set_flag(z, result == 0);
    return result;
}

bool Cpu::compare8(BinOpt8 arg) {
    // compares are just a subtraction
    add8(arg, true);
    return registers.get_flag(z) == 0;
}

uint8_t Cpu::step8(bool increment) {
    bool old_flag = registers.get_flag(c);
    uint8_t res = add8(static_cast<uint8_t>(1), increment);
    registers.set_flag(c, old_flag);
    return res;
}

uint16_t Cpu::add16(Register16 dest, Register16 operand) {
    if(operand == PC || operand == AF) throw std::logic_error("Cannot use accumulator or program counter as argument for add16");
    registers.set_flag(n, 0);

    uint16_t unpacked_arg_reg = registers.read(operand);
    uint16_t unpacked_dest_reg = registers.read(dest);
    int res = unpacked_dest_reg + unpacked_arg_reg;

    registers.set_flag(c, res > 0xFFFF);
    registers.set_flag(h, (unpacked_dest_reg & BIT_11_MASK) + (unpacked_arg_reg & BIT_11_MASK) > BIT_11_MASK);
    return static_cast<uint16_t>(res);
}

uint16_t Cpu::add_sp_signed(int8_t operand){
    registers.set_flag(n, 0);
    registers.set_flag(z, 0);
    
    uint16_t unpacked = registers.read(SP);
    int res = unpacked + operand;
    // set flags like an add8 with subtraction set as whether or not the operand is less than 0, carry=false, 
    // and the optional dest arg as the SP (the function will cast away the upper byte)
    add8(static_cast<uint8_t>(operand), operand < 0, false, SP);
    return res;
}

uint16_t Cpu::step16(Register16 dest, bool increment) {
    return registers.read(dest) + (increment ? 1 : -1);
}

//see this: https://rgbds.gbdev.io/docs/v0.9.2/gbz80.7#DAA
uint8_t Cpu::decimal_adjust_acc() {
    uint8_t adjustment = 0;
    uint8_t reg_a = registers.read_half(A);
    uint8_t res = 0;
    bool c_flag = false;
    if(registers.get_flag(n)) {
        if(registers.get_flag(h)) adjustment += DECIMAL_ADJUST_LOW;
        if(registers.get_flag(c)) adjustment += DECIMAL_ADJUST_HIGH;
        res = add8(adjustment, true);
    } else {
        if(registers.get_flag(h) || reg_a & 0xF > LOW_NIBBLE_THRESHOLD) adjustment += DECIMAL_ADJUST_LOW;
        if(registers.get_flag(c) || reg_a > FULL_BYTE_THRESHOLD){ 
            adjustment += DECIMAL_ADJUST_HIGH;
            c_flag = true;
        };
        res = adjustment + res;
    }
    registers.set_flag(z, res == 0);
    registers.set_flag(h, 0);
    registers.set_flag(c, c_flag);
    // n remains the same
    return res;
}

bool Cpu::complement_carry_flag() {
    registers.set_flag(n, 0);
    registers.set_flag(h, 0);
    registers.set_flag(c, !registers.get_flag(c));
    return registers.get_flag(c);
}

bool Cpu::set_carry_flag() {
    registers.set_flag(n, 0);
    registers.set_flag(h, 0);
    registers.set_flag(c, 1);
    return registers.get_flag(c);
}

uint8_t Cpu::complement_accumulator() {
    uint8_t flipped = ~registers.read_half(A);
    registers.set_flag(n, 1);
    registers.set_flag(h, 1);
    registers.write_half(A, flipped);
    return flipped;
}

