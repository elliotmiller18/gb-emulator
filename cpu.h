#pragma once

#include "register.h"
#include "memory.h"
#include <vector>
#include <array>
#include <variant>
#include <stdexcept>

constexpr int NUM_1_BYTE_OPCODES = 256;
struct GraphicsObject;

class Cpu {
public:
    Registers registers;
    Memory memory;
    int current_opcode;
    int batched_cycles = 0;
    bool stop_mode = false;
    bool halt_mode = false;
    bool buggy_halt_mode = false;
    // used to test and capture boot rom state
    bool booting = false;
    /// disabled on boot
    bool ime = false;
    bool debug;

    bool queued_interrupt_enable = false;

    Cpu(const char* filename) : registers(Registers()), memory(registers, filename, *this), debug(false) {}
    Cpu() : registers(Registers()), memory(registers, *this) {}
    ~Cpu() {}

    enum LogicalOperation {
        AND,
        OR,
        XOR,
    };
    
// UTILS
    uint8_t fetch_and_inc();
    uint16_t fetch_and_inc_imm_16();
    void print_state();
    uint8_t get_imm8_from_bits(int bits);
    uint16_t get_e_or_f_prefixed_ld_addr(int opcode);
    void write_to_dest8(RegisterOpt dest, uint8_t imm8);
    // ime is true and the bitwise and of the two interrupt control bytes != 0 means pending interrupt
    
// INTERRUPTS
    bool pending_interrupt();
    void request_interrupt(uint8_t control_bit);
    void poll_input();
    
// CPU ACTIONS
    void run();
    int step();
    bool check_and_handle_interrupts();
    void boot();
    void cycle(int mcycles);
    void sleep(std::chrono::nanoseconds time_to_sleep);

// GRAPHICS
    void graphics_handler();
    void draw_line();
    void draw_objects(scanline_t& scanline, std::vector<GraphicsObject>& objects) const;
    std::vector<GraphicsObject> oam_scan_line() const;
    void oam_dma_transfer(uint8_t source_addr);
    inline uint8_t get_lcd_control() const;
    
// REGULAR INSTRUCTION HANDLERS
    int invalid_opcode();
    int noop();
    int ld_imm16_to_reg16();
    int ld_acc_to_memory();
    int step16_handler();
    int step8_handler();
    int ld_imm8_to_dest8();
    int rotate_left_handler();
    int ld_sp_to_mem();
    int add_hl_handler();
    int ld_mem8_to_acc();
    int rotate_right_handler();
    int stop();
    int jr();
    int daa_handler();
    int complement_acc();
    int set_carry_flag();
    int complement_carry_flag();
    int ld_reg_or_memref_to_dest8();
    int halt();
    int add8_handler();
    int logical_op8_handler();
    int cp_handler();
    int ret();
    int pop();
    int jp();
    int call();
    int push();
    int rst();
    int cb_prefix();
    int e_prefixed_ldh();
    int add_sp_e8_handler();
    int f_prefixed_ldh();
    int di();
    int ld_add_sp_e8_to_hl();
    int ld_sp_hl();
    int ei();


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
    uint8_t shift_left(BinOpt8 operand);
    uint8_t shift_right(BinOpt8 operand, bool preserveBit7);
    /// @param carry if true, but 0 is set to bit 7, if false, bit 0 is set to CY (confusing, I know)
    uint8_t rotate_left(BinOpt8 operand, bool carry);
    /// @param carry if true, but 7 is set to bit 0, if false, bit 7 is set to CY (confusing, I know)
    uint8_t rotate_right(BinOpt8 operand, bool carry);
    /// swaps msb and lsb
    uint8_t swap(BinOpt8 operand);
};
