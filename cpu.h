#pragma once

#include "register.h"
#include "memory.h"
#include <array>
#include <variant>
#include <stdexcept>

inline constexpr int NUM_1_BYTE_OPCODES = 256;

class Cpu {
public:
    Registers registers;
    Memory memory;
    int current_opcode;
    bool debug;
    bool stop_mode;

    Cpu(const char* filename) : registers(Registers()), memory(registers, filename), debug(false) {}
    Cpu() : registers(Registers()), memory(registers) {}
    ~Cpu() {}

    enum LogicalOperation {
        AND,
        OR,
        XOR,
    };
    
// NON-INSTRUCTIONS
    uint8_t fetch_and_inc();
    uint16_t fetch_and_inc_imm_16();
    //TODO: implement (simulates one CPU cycle)
    void cycle(int cycles = 1);
    uint8_t get_current_opcode();
    void print_state();
    uint8_t get_imm8_from_bits(int bits);
    uint16_t get_e_or_f_prefixed_ld_addr(int opcode);
    void write_to_dest8(RegisterOpt dest, uint8_t imm8);
    void throttle_to_time(int machine_cycles);
    void run();
    int step();

// REGULAR INSTRUCTION HANDLERS
    void invalid_opcode();
    void noop();
    void ld_imm16_to_reg16();
    void ld_acc_to_memory();
    void step16_handler();
    void step8_handler();
    void ld_imm8_to_dest8();
    void rotate_left_handler();
    void ld_sp_to_mem();
    void add_hl_handler();
    void ld_mem8_to_acc();
    void rotate_right_handler();
    void stop();
    void jr();
    void daa_handler();
    void complement_acc();
    void set_carry_flag();
    void complement_carry_flag();
    void ld_reg_or_memref_to_dest8();
    //TODO: implement when we do interrupts
    void halt() {invalid_opcode();}
    void add8_handler();
    void logical_op8_handler();
    void cp_handler();
    //TODO: verify after research
    void ret();
    void pop();
    void jp();
    void call();
    void push();
    //TODO: implement later
    void rst() {invalid_opcode();}

    //TODO: implement when we do interrupts
    void reti() {invalid_opcode();}
    void cb_prefix();
    void e_prefixed_ldh();
    void add_sp_e8_handler();
    void f_prefixed_ldh();
    //TODO: implement when we do interrupts
    void di() {invalid_opcode();}
    void add_sp_e8_to_hl();
    void ld_sp_hl();
    //TODO: implement when we do interrupts
    void ei() {invalid_opcode();}


/// OLD INSTRUCTIONS, mostly just used for reference
// 8 bit arith operations
    uint8_t add8(BinOpt8 arg1, BinOpt8 arg2, bool subtraction, bool carry = false);
    uint8_t logical_operation8(BinOpt8 arg, LogicalOperation op);
    uint8_t step8(BinOpt8 arg, bool increment);
// 16 bit arith operations
    uint16_t add16(Register16 dest, Register16 operand);
    uint16_t add_sp_signed(int8_t operand);
    uint8_t decimal_adjust_acc();
// bit operations
    /// tests reg[bit] and sets the zero flag accordingly
    bool bit(int bit, BinOpt8 arg);
    /// sets provided bit to 0
    uint8_t reset_bit(int bit, BinOpt8 arg);
    /// sets provided bit to 1
    uint8_t set_bit(int bit, BinOpt8 arg);
    uint8_t shift_left(BinOpt8 operand);
    uint8_t shift_right(BinOpt8 operand, bool preserveBit7);
    /// @param carry if true, but 0 is set to bit 7, if false, bit 0 is set to CY (confusing, I know)
    uint8_t rotate_left(BinOpt8 operand, bool carry);
    /// @param carry if true, but 7 is set to bit 0, if false, bit 7 is set to CY (confusing, I know)
    uint8_t rotate_right(BinOpt8 operand, bool carry);
    /// swaps msb and lsb
    uint8_t swap(BinOpt8 operand);
};
