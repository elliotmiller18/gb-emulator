#include "cpu.h"
#include "register.h"
#include "memory.h"
#include "utils.h"
#include <stdexcept>
#include <iostream>
#include <string_view>

constexpr int BIT_11_MASK = 0x0FFF;
constexpr uint8_t DECIMAL_ADJUST_LOW = 0x6;
constexpr uint8_t DECIMAL_ADJUST_HIGH = 0x60;
constexpr uint8_t LOW_NIBBLE_THRESHOLD = 0x9;
constexpr uint8_t FULL_BYTE_THRESHOLD = 0x99;
constexpr std::string_view INVALID_ARG_MSG = "Unfortunately, there is no native 3 bit type in C++. Argument must be less than 0b111";

uint8_t Cpu::add8(BinOpt8 arg1, BinOpt8 arg2, bool subtraction, bool carry) {
    uint8_t unpacked1 = registers.unpack_binopt8(arg1);
    uint8_t unpacked2 = registers.unpack_binopt8(arg2);

    uint8_t carry_val = carry ? registers.get_flag(c) : 0;
    
    int res = subtraction ? unpacked1 - unpacked2 - carry_val : unpacked1 + unpacked2 + carry_val;

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

uint8_t Cpu::step8(BinOpt8 arg, bool increment) {
    bool old_flag = registers.get_flag(c);
    uint8_t res = add8(arg, static_cast<uint8_t>(1), !increment);
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
    //TODO: check this flagsetting behavior
    add8(static_cast<uint8_t>(operand), operand < 0, false, SP);
    return res;
}

// uint16_t Cpu::step16(Register16 dest, bool increment) {
//     return registers.read(dest) + (increment ? 1 : -1);
// }

//see this: https://rgbds.gbdev.io/docs/v0.9.2/gbz80.7#DAA
uint8_t Cpu::decimal_adjust_acc() {
    uint8_t adjustment = 0;
    uint8_t reg_a = registers.read_half(A);
    uint8_t res = reg_a;
    bool c_flag = false;
    if(registers.get_flag(n)) {
        if(registers.get_flag(h)) adjustment += DECIMAL_ADJUST_LOW;
        if(registers.get_flag(c)) adjustment += DECIMAL_ADJUST_HIGH;
        res = add8(A, adjustment, true);
    } else {
        if(registers.get_flag(h) || (reg_a & 0xF) > LOW_NIBBLE_THRESHOLD) adjustment += DECIMAL_ADJUST_LOW;
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

bool Cpu::bit(int bit, BinOpt8 arg) {
    if(bit > 0b111) throw std::invalid_argument(INVALID_ARG_MSG.data());
    // for some reason ..?
    uint8_t unpacked = registers.unpack_binopt8(arg);
    bool test = get_bit(unpacked, bit);
    registers.set_flag(z, !test);
    registers.set_flag(n, 0);
    registers.set_flag(h, 1);
    return test;
}

uint8_t Cpu::shift_left(BinOpt8 operand) {
    uint8_t unpacked = registers.unpack_binopt8(operand);
    uint8_t res = unpacked << 1;
    registers.set_flag(z, res == 0);
    registers.set_flag(h, 0);
    registers.set_flag(n, 0);
    // contents of original bit 7
    registers.set_flag(c, (unpacked & (1 << 7)));
    return res;
}

uint8_t Cpu::shift_right(BinOpt8 operand, bool arith) {
    uint8_t unpacked = registers.unpack_binopt8(operand);
    uint8_t res = unpacked >> 1;

    if(arith) res |= unpacked & 0x80;

    registers.set_flag(z, res == 0);
    registers.set_flag(h, 0);
    registers.set_flag(n, 0);
    // contents of original bit 0
    registers.set_flag(c, unpacked & 1);
    return res;
}

uint8_t Cpu::rotate_left(BinOpt8 operand, bool carry) {
    uint8_t unpacked = registers.unpack_binopt8(operand);
    uint8_t res = unpacked << 1;
    if(carry) res |= unpacked >> 7;
    else res |= static_cast<int>(registers.get_flag(c));

    registers.set_flag(z, res == 0);
    registers.set_flag(h, 0);
    registers.set_flag(n, 0);
    // contents of original bit 0
    registers.set_flag(c, get_bit(unpacked, 7));
    return res;
}

uint8_t Cpu::rotate_right(BinOpt8 operand, bool carry) {
    uint8_t unpacked = registers.unpack_binopt8(operand);
    uint8_t res = unpacked >> 1;
    if(carry) res |= (unpacked & 1) << 7 ;
    else res |= static_cast<int>(registers.get_flag(c)) << 7;

    registers.set_flag(z, res == 0);
    registers.set_flag(h, 0);
    registers.set_flag(n, 0);
    // contents of original bit 0
    registers.set_flag(c, unpacked & 1);
    return res;
}
/// swaps msb and lsb
uint8_t Cpu::swap(BinOpt8 operand) {
    uint8_t unpacked = registers.unpack_binopt8(operand);
    registers.set_flag(z, unpacked == 0);
    registers.set_flag(h, 0);
    registers.set_flag(n, 0);
    registers.set_flag(c, 0);
    // contents of original bit 0
    return (lsb_16(unpacked) << 4) | msb_16(unpacked);
}

