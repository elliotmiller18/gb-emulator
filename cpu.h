#pragma once

#include "register.h"
#include "memory.h"
#include <array>
#include <variant>
#include <stdexcept>

/// the value that programs expect the SP to be initialized to
constexpr uint16_t STACK_STARTING_VALUE = 0xFFFE;
/// the pointer to the top of VRAM
constexpr uint16_t VRAM_TOP = 0x9FFF;

class Cpu {
public:
    Registers registers;
    Memory memory;
    int current_opcode;

    Cpu(const char* filename) : registers(Registers()), memory(registers, filename) {}
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
    int execute();
    //TODO: implement (simulates one CPU cycle)
    void cycle(int cycles = 1);
    uint8_t get_current_opcode();
    void print_state();
    uint8_t get_imm8_from_arg_bits(int bits);

// INSTRUCTION HANDLERS AND SIMPLE INSTRUCTIONS, SORTED BY OPCODE

// handler means it uses something in instructions.cpp
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
    void cpl_handler();
    void scf_handler();
    void ccf_handler();
    void halt();
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
    void rst();

    //TODO: implement when we do interrupts
    void reti();
    //TODO: implement when we implement prefixed instructions
    void cb_prefix();
    void e_prefixed_ld();
    void add_sp_e8_handler();
    void f_prefixed_ld();
    //TODO: implement
    void di();  
    void add_sp_e8_to_hl();
    void ld_sp_hl();
    //TODO: implement
    void ei();
    


// 8 bit transfer operations
    //NOTE: unimplemented for now, left here in case we want to use it for better hardware emulation
    uint8_t load_to_reg8(Register8 dest, BinOpt8 arg);
    uint8_t load_to_mem8(BinOpt8 arg);
// 16 bit transfer operations
    uint16_t load_to_reg16(Register16 dest, BinOpt16 arg);
    // not implemented for now as the behavior is identical to add_sp_signed, the only difference is how the result is used
    uint16_t ldhl_sp(int8_t operand);
    /// @brief pushes a value onto stack and decrements SP by 2
    /// @param arg register to be stored onto the stack
    /// @return new value of SP 
    uint16_t push(Register16 arg);
    /// @brief pops a value off of stack, stores it in dest and increments SP by 2
    /// @param dest destination regiser
    /// @return new value of SP 
    uint16_t pop(Register16 dest);
// 8 bit arith operations
    uint8_t add8(BinOpt8 arg1, BinOpt8 arg2, bool subtraction, bool carry = false);
    uint8_t logical_operation8(BinOpt8 arg, LogicalOperation op);
    uint8_t step8(BinOpt8 arg, bool increment);
// 16 bit arith operations
    uint16_t add16(Register16 dest, Register16 operand);
    uint16_t add_sp_signed(int8_t operand);
    uint16_t step16(Register16 dest, bool increment);
    bool complement_carry_flag();
    bool set_carry_flag();
    uint8_t complement_accumulator();
    uint8_t decimal_adjust_acc();
// control flow
    // ---- TODO: left unimplemented for now because they're so simple ---
    uint16_t jump(uint16_t addr);
    uint16_t jump_cc(uint16_t addr);
    // ---- unimplemented
    void call(uint16_t faddr, bool condition = false);
    [[deprecated("Ret is deprecated, Use ret handler in handlers.cpp instead")]]
    void ret(bool conditional = false, bool interrupt = false);
// bit operations
    /// tests reg[bit] and sets the zero flag accordingly
    bool bit(int bit, BinOpt8 arg);
    /// sets provided bit to 0
    uint8_t res(int bit, BinOpt8 arg);
    /// sets provided bit to 1
    uint8_t set(int bit, BinOpt8 arg);
    uint8_t shift_left(BinOpt8 operand);
    uint8_t shift_right(BinOpt8 operand, bool preserveBit7);
    /// @param carry if true, but 0 is set to bit 7, if false, bit 0 is set to CY (confusing, I know)
    uint8_t rotate_left(BinOpt8 operand, bool carry);
    /// @param carry if true, but 7 is set to bit 0, if false, bit 7 is set to CY (confusing, I know)
    uint8_t rotate_right(BinOpt8 operand, bool carry);
    /// swaps msb and lsb
    uint8_t swap(BinOpt8 operand);
};
