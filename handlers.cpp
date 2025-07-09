
#include "cpu.h"
#include "register.h"
#include "memory.h"
#include "utils.h"
#include <stdexcept>
#include <iostream>

//TODO: double check if we can batch cycle like we do in ld_reg_16 or we need to cycle after each operation

void Cpu::noop() {cycle();}

void Cpu::ld_imm16_to_reg16() {
    uint8_t opcode = get_current_opcode();
    Register16 dest = get_register16_from_opcode_bits(get_bits_in_range(opcode, 4, 5));
    uint8_t msb = fetch_and_inc();
    uint8_t lsb = fetch_and_inc();
    registers.write(dest, combine_bytes(msb, lsb));
    cycle(3);
}

void Cpu::ld_acc_to_memory(){
    // we'll only be here with opcode 00xx0010
    uint8_t opcode = get_current_opcode();
    switch(opcode >> 4) {
        case 0b00:
            memory.write_byte(BC, A);
            break;
        case 0b01:
            memory.write_byte(DE, A);
            break;
        case 0b10:
            memory.write_byte(HL, A);
            registers.write(HL, static_cast<uint16_t>(registers.read(HL) + 1));
            break;
        case 0b11:
            memory.write_byte(HL, A);
            registers.write(HL, static_cast<uint16_t>(registers.read(HL) - 1));
            break;
    }
    cycle(2);
}

void Cpu::inc16_handler(){
    Register16 target = get_register16_from_opcode_bits(get_bits_in_range(get_current_opcode(), 4, 5));
    registers.write(target, static_cast<uint16_t>(registers.read(target) + 1));
    cycle(2);
}

void Cpu::step8_handler() {
    int opcode = get_current_opcode();
    int dest_bits = get_bits_in_range(opcode, VERTICAL_VAL8_START, VERTICAL_VAL8_END);
    // the last nibble of inc8 is 0b100, dec8 is 0b101
    bool increment = get_bit(opcode, 0) == 0;
    RegisterOpt dest_opt = get_dest8_from_bits(dest_bits);
    if(std::get_if<Register16>(&dest_opt)) {
        uint8_t byte = memory.read_byte(registers.read(HL));
        memory.write_byte(registers.read(HL), step8(byte, increment));
        cycle(3);
        return;
    }
    Register8 dest = std::get<Register8>(dest_opt);
    registers.write_half(dest, step8(dest, increment));
    cycle(1);
}

void Cpu::ld_imm8_to_dest8() {
    int opcode = get_current_opcode();
    uint8_t imm8;
    RegisterOpt dest = get_dest8_from_bits(get_bits_in_range(opcode, VERTICAL_VAL8_START, VERTICAL_VAL8_END));
    if(msb_8(opcode) <= 0x3) {
        imm8 = fetch_and_inc();
    } else {
        // bottom 3 bits
        RegisterOpt val = get_dest8_from_bits(opcode % 8);
        if(std::get_if<Register16>(&val)) imm8 = memory.read_byte(registers.read(HL));
        else imm8 = registers.read_half(std::get<Register8>(val));
    }

    if(std::get_if<Register16>(&dest)) memory.write_byte(registers.read(HL), imm8);
    else registers.write_half(std::get<Register8>(dest), imm8);
}

void Cpu::rotate_left_handler() {
    // opcode is 0x000*111 where bit 4 corresponds to whether or not carry is enabled (see func def)
    registers.write_half(A, rotate_left(A, get_bit(get_current_opcode(), 4)));
}

void 