#include "cpu.h"
#include "register.h"
#include "memory.h"
#include "utils.h"
#include <stdexcept>
#include <iostream>

//TODO: double check if we can batch cycle like we do in ld_reg_16 or we need to cycle after each operation

void Cpu::noop() {}

void Cpu::ld_imm16_to_reg16() {
    Register16 dest = get_register16_from_opcode(current_opcode);
    registers.write(dest, fetch_and_inc_imm_16());
}

void Cpu::ld_acc_to_memory(){
    // we'll only be here with opcode 00xx0010
    switch(msb_8(current_opcode)    ) {
        case 0b00:
            memory.write_byte(BC, A);
            return;
        case 0b01:
            memory.write_byte(DE, A);
            return;
        case 0b10:
            memory.write_byte(HL, A);
            registers.write(HL, static_cast<uint16_t>(registers.read(HL) + 1));
            return;
        case 0b11:
            memory.write_byte(HL, A);
            registers.write(HL, static_cast<uint16_t>(registers.read(HL) - 1));
            return;
    }
}

void Cpu::step16_handler(){
    // this instruction does not affect flags
    Register16 dest = get_register16_from_opcode(current_opcode);
    uint16_t unpacked = registers.read(dest);
    //0x3 is inc, 0xB is dec
    unpacked = lsb_8(current_opcode) == 0x3 ? unpacked + 1 : unpacked - 1;
    registers.write(dest, unpacked);
}

void Cpu::step8_handler() {
    int dest_bits = get_bits_in_range(current_opcode, VERTICAL_VAL8_START, VERTICAL_VAL8_END);
    RegisterOpt dest = get_dest8_from_bits(dest_bits);
    uint8_t unpacked = get_imm8_from_bits(dest_bits);
    // the last nibble of inc8 is 0b100, dec8 is 0b101
    bool increment = get_bit(current_opcode, 0) == 0;
    write_to_dest8(dest, step8(unpacked, increment));
}

void Cpu::ld_imm8_to_dest8() {
    // next immediate in memory to dest calculated from opcode
    write_to_dest8(get_dest8_from_bits(get_bits_in_range(current_opcode, VERTICAL_VAL8_START, VERTICAL_VAL8_END)), fetch_and_inc());
}

void Cpu::rotate_left_handler() {
    // opcode is 0x000*0111 where bit 4 corresponds to whether or not carry is enabled (see func def)
    registers.write_half(A, rotate_left(A, get_bit(current_opcode, 4) == 0));
    // the non cb prefixed rotate left resets 0
    registers.set_flag(z, 0);
}

void Cpu::ld_sp_to_mem() {
    uint16_t addr = fetch_and_inc_imm_16();
    memory.write_word(addr, SP);
}

void Cpu::add_hl_handler() {
    // this instruction is 0b00xx1001 so we can safely do this
    Register16 operand = get_register16_from_opcode(current_opcode);
    registers.write(HL, add16(HL, operand));
}

void Cpu::ld_mem8_to_acc(){
    switch(msb_8(current_opcode)) {
        case 0:
            registers.write_half(A, memory.read_byte(BC));
            break;
        case 1:
            registers.write_half(A, memory.read_byte(DE));
            break;
        case 2:
            registers.write_half(A, memory.read_byte(HL));
            registers.write(HL, static_cast<uint16_t>(registers.read(HL) - 1));
            break;
        case 3:
            registers.write_half(A, memory.read_byte(HL));
            registers.write(HL, static_cast<uint16_t>(registers.read(HL) + 1));
            break;
    } 
}

void Cpu::rotate_right_handler() {
    // opcode is 0x000*1111 where bit 4 corresponds to whether or not carry is enabled (see func def)
    registers.write_half(A, rotate_right(A, get_bit(current_opcode, 4) == 0));
    // the non cb prefixed rotate right resets 0
    registers.set_flag(z, 0);
}

void Cpu::stop() {
    registers.disable_interrupts();
    //there's junk data after
    fetch_and_inc();
}

void Cpu::jr() {
    bool condition;
    int8_t offset = static_cast<int8_t>(fetch_and_inc());
    switch(msb_8(get_current_opcode())) {
        case 1:
            condition = true;
            break;
        case 2:
            condition = registers.get_flag(z);
            break;
        case 3:
            condition = registers.get_flag(c);
            break;
        default:
            // unconditional jump
            throw std::runtime_error("Invalid opcode in JR");
            break;
    }
    if(lsb_8(current_opcode) == 0) condition = !condition;
    if(condition) {
        registers.write(PC, static_cast<uint16_t>(registers.read(PC) + offset));
    }   
}

void Cpu::daa_handler() {
    registers.write_half(A, decimal_adjust_acc());
}

void Cpu::complement_acc() {
    uint8_t flipped = ~registers.read_half(A);
    registers.set_flag(n, 1);
    registers.set_flag(h, 1);
    registers.write_half(A, flipped);
}

void Cpu::set_carry_flag() {
    registers.set_flag(n, 0);
    registers.set_flag(h, 0);
    registers.set_flag(c, 1);
}

void Cpu::complement_carry_flag() {
    registers.set_flag(n, 0);
    registers.set_flag(h, 0);
    registers.set_flag(c, !registers.get_flag(c));
}

void Cpu::ld_reg_or_memref_to_dest8() {
    //TODO: double check this
    write_to_dest8(get_dest8_from_bits(get_bits_in_range(current_opcode, VERTICAL_VAL8_START, VERTICAL_VAL8_END)), 
                   get_imm8_from_bits(current_opcode % 8));
}


void Cpu::add8_handler() {
    uint8_t arg;
    if(msb_8(current_opcode) > 0xB) arg = fetch_and_inc();
    else arg = get_imm8_from_bits(current_opcode % 8);
    //add is 0b1000xxxx, where the first 3 bits are the operand the last bit of the lower nibble is the carry flag
    bool carry = get_bit(current_opcode, 3);
    //sub is 0b1001xxxx
    bool subtraction = get_bit(current_opcode, 4);
    registers.write_half(A, add8(A, arg, subtraction, carry));
}

void Cpu::logical_op8_handler() {
    // all xor instructions are and instructions with bit 3 set, and all ors are and instructions with 1 added to the msb
    // this is a bit confusing to follow but if you see the table it'll make sense
    LogicalOperation op = AND;
    int msb = msb_8(current_opcode);
    if(msb == 0xF || msb == 0xB) op = OR;
    else if(lsb_8(current_opcode) > 7) op = XOR;

    uint8_t arg = msb_8(current_opcode) > 0xB ? fetch_and_inc() : get_imm8_from_bits(current_opcode % 8);

    registers.write_half(A, logical_operation8(arg, op));
}

void Cpu::cp_handler() {
    uint8_t arg = current_opcode == 0xFE ? fetch_and_inc() :  get_imm8_from_bits(current_opcode % 8);
    add8(A, arg, true);
}

void Cpu::ret() {
    bool condition = true;
    if((current_opcode & 0b111) == 0){
        condition = msb_8(current_opcode) == 0xC ? registers.get_flag(z) : registers.get_flag(c);
        // ret cc or ret not cc
        condition = lsb_8(current_opcode) == 0x8 ? condition : !condition;
    }
    if(!condition) return;
    registers.write(PC, memory.read_word_and_inc_sp());
}

void Cpu::pop() {
    Register16 dest = get_register16_from_opcode(current_opcode);
    // in this instruction 11 means AF not SP
    if(dest == SP) dest = AF;
    registers.write(dest, memory.read_word_and_inc_sp());
}

void Cpu::jp() {
    bool condition;
    if(lsb_8(current_opcode) == 0x3 || lsb_8(current_opcode) == 0x9) condition = true;
    else if(msb_8(current_opcode) == 0xC) condition = registers.get_flag(z);
    else if(msb_8(current_opcode) == 0xD) condition = registers.get_flag(c);
    else throw std::runtime_error("Invalid opcode in jp");
    if(lsb_8(current_opcode) == 0x2) condition = !condition;

    //jp imm16 or jp HL are the two options
    if(condition) registers.write(PC, current_opcode == 0xE9 ? registers.read(HL) : fetch_and_inc_imm_16());
}

void Cpu::call() {
    bool condition = true;
    if((current_opcode & 0b11) == 0) {
        condition = msb_8(current_opcode) == 0xC ? registers.get_flag(z) : registers.get_flag(c);
        // call cc or call not cc
        condition = lsb_8(current_opcode) == 0x4 ? condition : !condition;
    }
    if(!condition) return;
    uint16_t addr = fetch_and_inc_imm_16();
    memory.write_word_and_dec_sp(PC);
    registers.write(PC, addr);
}

void Cpu::push() {
    Register16 arg = get_register16_from_opcode(current_opcode);
    // in this instruction 11 means AF not SP
    if(arg == SP) arg = AF;
    memory.write_word_and_dec_sp(arg);
}

void Cpu::e_prefixed_ld() {
    memory.write_byte(get_e_or_f_prefixed_ld_addr(current_opcode), A);
}

void Cpu::add_sp_e8_handler() {
    registers.write(SP, add_sp_signed(static_cast<int8_t>(fetch_and_inc())));
}

void Cpu::f_prefixed_ld() {
    registers.write_half(A, memory.read_byte(get_e_or_f_prefixed_ld_addr(current_opcode)));
}

void Cpu::add_sp_e8_to_hl() {
    registers.write(HL, add_sp_signed(static_cast<int8_t>(fetch_and_inc())));
}

void Cpu::ld_sp_hl() {
    registers.write(SP, HL);
}

void Cpu::cb_prefix() {
    uint8_t opcode = fetch_and_inc();
    uint8_t arg = get_imm8_from_bits(opcode % 8);

    switch(msb_8(opcode)) {
        case 0x0:
        case 0x1:
            bool carry = msb_8(opcode) == 0;
            arg = lsb_8(opcode) <= 0x7 ? rotate_left(arg, carry) : rotate_right(arg, carry);
            break;
        case 0x2:
            //0x2 is for logical shifts (the false means not arith shift)
            arg = lsb_8(opcode) <= 0x7 ? shift_left(arg) : shift_right(arg, false);
            break;
        case 0x3:
            //0x3 is for arith shifts
            arg = lsb_8(opcode) <= 0x7 ? swap(arg) : shift_right(arg, true);
            break;
        case 0x4:
        case 0x5:
        case 0x6:
        case 0x7:
            bit(get_bits_in_range(opcode, 3, 5), arg);
            return;
        case 0x8:
        case 0x9:
        case 0xA:
        case 0xB:
            arg = reset_bit(get_bits_in_range(opcode, 3, 5), arg);
            break;
        case 0xC:
        case 0xD:
        case 0xE:
        case 0xF:
            arg = set_bit(get_bits_in_range(opcode, 3, 5), arg);
            break;
        default:
            throw std::runtime_error("msb_8 returned something unexpected");
    }

    std::visit([arg, this](auto&& dest) mutable {
        using arg_type = std::decay_t<decltype(dest)>;
        if constexpr(std::is_same_v<arg_type, Register8>) registers.write_half(dest, arg);
        else if constexpr(std::is_same_v<arg_type, Register16>) memory.write_byte(HL, arg);
        else throw std::runtime_error("Invalid destination in cb_prefix");
    }, get_dest8_from_bits(opcode % 8));
}