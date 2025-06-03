#include "cpu.h"
#include "rom.h"
#include "register.h"
#include <stdexcept>

constexpr int BIT_11_MASK = 0x0FFF;

uint8_t Cpu::add8(BinOpt8 arg, bool subtraction, bool carry) {
    uint8_t unpacked = registers.unpack_binopt8(arg);
    uint8_t reg_a = registers.read_half(A);
    uint8_t carry_val = registers.get_flag(c) && carry ? 1 : 0;

    int res;
    if(subtraction) {
        res = reg_a - unpacked - carry_val;
        registers.set_flag(c, reg_a < unpacked + carry_val);
        registers.set_flag(h, (reg_a & 0xF) < (unpacked & 0xF) + carry_val);
    } else {  
        res = reg_a + unpacked + carry_val;  
        registers.set_flag(c, res > 0xFF);
        // do the lower 4 bits of both numbers add up to more than 0xF
        registers.set_flag(h, (reg_a & 0xF) + (unpacked & 0xF) + carry_val > 0xF);
    }

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
    registers.set_flag(c, res > 0xFFFF);
    //TODO: check this flag logic in discord, chatgpt doesn't like it but the reference manual seems to confirm it
    registers.set_flag(h, (unpacked & BIT_11_MASK) + operand > BIT_11_MASK);
    return res;
}

uint16_t Cpu::step16(Register16 dest, bool increment) {
    return registers.read(dest) + (increment ? 1 : -1);
}