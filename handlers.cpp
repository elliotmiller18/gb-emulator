
#include "cpu.h"
#include "register.h"
#include "memory.h"
#include "utils.h"
#include <stdexcept>
#include <iostream>

//TODO: double check if we can batch cycle like we do in ld_reg_16 or we need to cycle after each operation

void Cpu::noop() {}

void Cpu::ld_imm16_to_reg16() {
    uint8_t opcode = get_current_opcode();
    Register16 dest = get_register16_from_opcode_bits(get_bits_in_range(opcode, 4, 5));
    registers.write(dest, fetch_and_inc_imm_16());
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
}

void Cpu::step16_handler(){
    Register16 target = get_register16_from_opcode_bits(get_bits_in_range(get_current_opcode(), 4, 5));
    uint16_t original = registers.read(target);
    //0x3 is inc, 0xB is dec
    if(lsb_8(get_current_opcode()) == 0x3) registers.write(target, ++original); 
    else registers.write(target, --original); 
}

void Cpu::step8_handler() {
    int opcode = get_current_opcode();
    int dest_bits = get_bits_in_range(opcode, VERTICAL_VAL8_START, VERTICAL_VAL8_END);
    // the last nibble of inc8 is 0b100, dec8 is 0b101
    bool increment = get_bit(opcode, 0) == 0;
    RegisterOpt dest_opt = get_dest8_from_bits(dest_bits);
    if(std::get_if<Register16>(&dest_opt)) {
        uint8_t byte = memory.read_byte(HL);
        memory.write_byte(HL, step8(byte, increment));
        return;
    }
    Register8 dest = std::get<Register8>(dest_opt);
    registers.write_half(dest, step8(dest, increment));
}

void Cpu::ld_imm8_to_dest8() {
    int opcode = get_current_opcode();
    uint8_t imm8;
    RegisterOpt dest = get_dest8_from_bits(get_bits_in_range(opcode, VERTICAL_VAL8_START, VERTICAL_VAL8_END));
    if(msb_8(opcode) <= 0x3) {
        if(lsb_8(opcode) != 0x6 && lsb_8(opcode) != 0xE) throw std::invalid_argument("Bad opcode in ld_imm8_to_dest8");
        imm8 = fetch_and_inc();
    } else {
        imm8 = get_imm8_from_arg_bits(opcode % 8);
    }

    if(std::get_if<Register16>(&dest)) {
        memory.write_byte(HL, imm8);
    }
    else registers.write_half(std::get<Register8>(dest), imm8);
}

void Cpu::rotate_left_handler() {
    // opcode is 0x000*0111 where bit 4 corresponds to whether or not carry is enabled (see func def)
    registers.write_half(A, rotate_left(A, get_bit(get_current_opcode(), 4) == 0));
}

void Cpu::ld_sp_to_mem() {
    uint16_t addr = fetch_and_inc_imm_16();
    memory.write_word(addr, SP);
}

void Cpu::add_hl_handler() {
    // this instruction is 0b00xx1001 so we can safely do this
    Register16 operand = get_register16_from_opcode_bits(get_current_opcode() >> 4);
    registers.write(HL, add16(HL, operand));
}

void Cpu::ld_mem8_to_acc(){
    switch(get_current_opcode() >> 4) {
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
    registers.write_half(A, rotate_right(A, get_bit(get_current_opcode(), 4) == 0));
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
        case 2:
            condition = registers.get_flag(z);
            break;
        case 3:
            condition = registers.get_flag(c);
            break;
        default:
            // unconditional jump
            condition = true;
            break;
    }
    if(lsb_8(get_current_opcode()) == 0) condition = !condition;
    if(condition) {
        registers.write(PC, static_cast<uint16_t>(registers.read(PC) + offset));
    }
}

void Cpu::daa_handler() {
    registers.write_half(A, decimal_adjust_acc());
}

void Cpu::cpl_handler() {
    registers.write_half(A, complement_accumulator());
}

void Cpu::scf_handler() {
    cycle(1);
    set_carry_flag();
}

void Cpu::ccf_handler() {
    complement_carry_flag();
}

void Cpu::halt() {
    //TODO: implement when we have interrupts done
    registers.disable_interrupts();
}

void Cpu::add8_handler() {
    uint8_t opcode = get_current_opcode();
    uint8_t operand;
    if(msb_8(opcode) > 0xB) operand = fetch_and_inc();
    else operand = get_imm8_from_arg_bits(opcode % 8);
    //add is 0b1000xxxx, where the first 3 bits are the operand the last bit of the lower nibble is the carry flag
    bool carry = get_bit(opcode, 3);
    //sub is 0b1001xxxx
    bool subtraction = get_bit(opcode, 4);
    registers.write_half(A, add8(A, operand, subtraction, carry));
}

void Cpu::logical_op8_handler() {
    // the table is laid out weirdly ..
    LogicalOperation op = AND;
    int msb = msb_8(current_opcode);
    if(msb == 0xF || msb == 0xB) op = OR;
    else if(lsb_8(current_opcode) > 7) op = XOR;

    uint8_t operand;
    if(msb_8(current_opcode) > 0xB) operand = fetch_and_inc();
    else operand = get_imm8_from_arg_bits(current_opcode % 8);

    registers.write_half(A, logical_operation8(operand, op));
}

void Cpu::cp_handler() {
    add8(A, get_imm8_from_arg_bits(current_opcode % 8), true);
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
    int reg_bits = get_bits_in_range(current_opcode, 4, 5);
    Register16 dest = reg_bits == 0b11 ? AF : static_cast<Register16>(reg_bits + 1);
    registers.write(dest, memory.read_word_and_inc_sp());
}

void Cpu::jp() {
    bool condition;
    if(lsb_8(current_opcode) == 0x3 || lsb_8(current_opcode) == 0x9) condition = true;
    else if(msb_8(current_opcode) == 0xC) condition = registers.get_flag(z);
    else condition = registers.get_flag(c);
    if(lsb_8(current_opcode) == 0x2) condition = !condition;

    uint16_t new_addr;
    //JP HL
    if(current_opcode == 0xE9) new_addr = registers.read(HL);
    else new_addr = fetch_and_inc_imm_16();

    if(condition) registers.write(PC, new_addr);
}

void Cpu::call() {
    bool condition = true;
    if((current_opcode & 0b11) == 0) {
        condition = msb_8(current_opcode) == 0xC ? registers.get_flag(z) : registers.get_flag(c);
        // call cc or call not cc
        condition = lsb_8(current_opcode) == 0xC ? condition : !condition;
    }
    if(!condition) return;
    uint8_t addr = fetch_and_inc_imm_16();
    memory.write_word_and_dec_sp(PC);
    registers.write(PC, addr);
}

void Cpu::e_prefixed_ld() {
    uint16_t addr;
    switch(lsb_8(current_opcode)) {
        case(0x0):
            addr = fetch_and_inc() + 0xFF00;
            break;
        case(0x2):
            addr = registers.read_half(C) + 0xFF00;
            break;
        case(0xA):
            addr = fetch_and_inc_imm_16();
            break;
        default:
            throw std::logic_error("Invalid opcode");
    }
    memory.write_byte(addr, registers.read_half(A));
}

void Cpu::add_sp_e8_handler() {
    registers.write(SP, add_sp_signed(static_cast<int8_t>(fetch_and_inc())));
}

void Cpu::f_prefixed_ld() {
    uint16_t addr;
    switch(lsb_8(current_opcode)) {
        case(0x0):
            addr = fetch_and_inc() + 0xFF00;
            break;
        case(0x2):
            addr = registers.read_half(C) + 0xFF00;
            break;
        case(0xA):
            addr = fetch_and_inc_imm_16();
            break;
        default:
            throw std::logic_error("Invalid opcode");
    }
    registers.write_half(A, memory.read_byte(addr));
}

void Cpu::add_sp_e8_to_hl() {
    registers.write(HL, add_sp_signed(static_cast<int8_t>(fetch_and_inc())));
}

void Cpu::ld_sp_hl() {
    registers.write(SP, HL);
}