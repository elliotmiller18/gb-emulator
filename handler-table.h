#include "cpu.h"

using opcode_handler = void (Cpu::*)();
constexpr int NUM_OPCODES = 512;

static constexpr opcode_handler opcode_table[NUM_OPCODES] = {
    /*0x00, NOP*/        &Cpu::noop,
    /*0x01, LD BC, d16*/ &Cpu::ld_imm16_to_reg16,
    /*0x02, LD [BC] A*/  &Cpu::ld_acc_to_memory,
    /*0x03, INC BC*/     &Cpu::step16_handler,
    /*0x04, INC B*/      &Cpu::step8_handler,
    /*0x05, DEC B*/      &Cpu::step8_handler,
};