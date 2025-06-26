#include "cpu.h"
#include "register.h"
#include "utils.h"
#include <stdexcept>

const std::string INVALID_ARG_MSG = "Unfortunately, there is no native 3 bit type in C++. Argument must be less than 0b111";

bool Cpu::bit(int bit, BinOpt8 arg) {
    if(bit > 0b111) throw std::invalid_argument(INVALID_ARG_MSG);
    registers.set_flag(n, 0);
    // for some reason ..?
    registers.set_flag(h, 1);
    uint8_t unpacked = registers.unpack_binopt8(arg);
    bool test = (unpacked & (1 << bit)) == 0;
    registers.set_flag(z, test);
    return test;
}

uint8_t Cpu::res(int bit, BinOpt8 arg) {
    if(bit > 0b111) throw std::invalid_argument(INVALID_ARG_MSG);
    uint8_t unpacked = registers.unpack_binopt8(arg);
    return (unpacked & !(1 << bit));
}

uint8_t Cpu::set(int bit, BinOpt8 arg) {
    if(bit > 0b111) throw std::invalid_argument(INVALID_ARG_MSG);
    uint8_t unpacked = registers.unpack_binopt8(arg);
    return (unpacked & (1 << bit));
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

uint8_t Cpu::shift_right(BinOpt8 operand, bool preserveBit7) {
    uint8_t unpacked = registers.unpack_binopt8(operand);
    uint8_t res = unpacked >> 1;

    if(preserveBit7) res |= unpacked & 0x80;

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
    else res |= static_cast<int>(carry);

    registers.set_flag(z, res == 0);
    registers.set_flag(h, 0);
    registers.set_flag(n, 0);
    // contents of original bit 0
    registers.set_flag(c, unpacked & 7);
    return res;
}

uint8_t Cpu::rotate_right(BinOpt8 operand, bool carry) {
    uint8_t unpacked = registers.unpack_binopt8(operand);
    uint8_t res = unpacked >> 1;
    if(carry) res |= (unpacked & 1) << 7 ;
    else res |= static_cast<int>(carry) << 7;

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
    return (lsb(unpacked) << 4) | msb(unpacked);
}
