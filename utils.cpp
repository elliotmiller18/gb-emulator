#include "utils.h"
#include "cpu.h"
#include "register.h"
#include <stdexcept>
#include <iostream>

std::string r16_name(int bits) {
    switch(bits) {
        case 0b00:  return "AF";
        case 0b01:  return "BC";
        case 0b10:  return "DE";
        case 0b11:  return "HL";
        case 0b100: return "SP";
        case 0b101: return "PC";
        default:    throw std::invalid_argument("There are only 6 16 bit registers");
    }
}

uint8_t Cpu::fetch_and_inc() {
    uint16_t old_pc = registers.read(PC);
    registers.adjust(PC, 1);
    return memory.read_byte(old_pc);
}

uint16_t Cpu::fetch_and_inc_imm_16() {
    uint8_t lsb = fetch_and_inc();
    uint8_t msb = fetch_and_inc();
    return combine_bytes(msb, lsb);
}

uint8_t Cpu::get_imm8_from_bits(int bits) {
    return std::visit([this](auto&& inner) -> uint8_t {
        using inner_t = std::decay_t<decltype(inner)>;
        if constexpr(std::is_same_v<Register16, inner_t>)      return memory.read_byte(registers.read(HL));
        else if constexpr (std::is_same_v<Register8, inner_t>) return registers.read_half(inner);
        else throw std::invalid_argument("Invalid RegisterOpt");
    }, get_dest8_from_bits(bits));
}

void Cpu::print_state() {
    for(size_t r = 0; r < NUM_REGISTERS; r++) {
        std::cout << "REGISTER " << r16_name(r) << ": 0x" << std::hex << registers.read(static_cast<Register16>(r)) << "\n";
    }
    std::cout << "MEM AFTER PC ";
    for(size_t m = registers.read(PC); m < registers.registers[PC] + 4; m++) {
        std::cout << std::hex << static_cast<int>(memory.memory[m]) << " ";
    }
    std::cout << "\n";
}

uint16_t Cpu::get_e_or_f_prefixed_ld_addr(int opcode) {
    switch(lsb_8(opcode)) {
        case(0x0):
            return fetch_and_inc() | 0xFF00;
        case(0x2):
            return registers.read_half(C) | 0xFF00;
        case(0xA):
            return fetch_and_inc_imm_16();
        default:
            throw std::logic_error("Invalid opcode");
    }
}

void Cpu::write_to_dest8(RegisterOpt dest, uint8_t imm8) {
    std::visit([imm8, this](auto&& dest) mutable {
        using dest_type = std::decay_t<decltype(dest)>;
        if constexpr(std::is_same_v<Register8, dest_type>) registers.write_half(dest, imm8);
        else if constexpr(std::is_same_v<Register16, dest_type>) memory.write_byte(HL, imm8);
        else throw std::runtime_error("Invalid destination in ld_reg_or_memref_to_dest8");
    }, dest);
}

bool Cpu::pending_interrupt() {
    return ime && (memory.read_byte(INTERRUPT_ENABLE_ADDR) & memory.read_byte(INTERRUPT_FLAG_ADDR));
}

// BIT OPERATIONS

/// returns 8 most significant bits
int msb_16(uint16_t val) {
    return val >> 8;
}

/// returns 4 most significant bits
int msb_8(uint8_t val) {
    return val >> 4;
}

/// returns 8 least significant bits
int lsb_16(uint16_t val) {
    return static_cast<uint8_t>(val);
}

/// returns 4 least significant bits
int lsb_8(uint8_t val) {
    return val & 0xF;
}


bool get_bit(int target, int position) {
    return (target >> position) & 1;
}

uint8_t reset_bit(int bit, uint8_t arg) {
    if(bit > 0b111) throw std::invalid_argument("Arg must be 3 bit integer");
    return (arg & ~(1 << bit));
}

uint8_t set_bit(int bit, uint8_t arg) {
    if(bit > 0b111) throw std::invalid_argument("Arg must be 3 bit integer");
    return (arg | (1 << bit));
}

uint8_t set_or_reset_bit(int bit, uint8_t arg, bool set) {
    return set ? set_bit(bit, arg) : reset_bit(bit, arg);
}

void reset_bit(int bit, uint8_t* arg) {
    *arg = reset_bit(bit, *arg);
}
void set_bit(int bit, uint8_t* arg) {
    *arg = set_bit(bit, *arg);
}

void set_or_reset_bit(int bit, uint8_t* arg, bool set) {
    *arg = set_or_reset_bit(bit, *arg, set);
}


/// start and end are both inclusive
int get_bits_in_range(unsigned int target, unsigned int start, unsigned int end) {
    if(start >= 16) throw std::invalid_argument("Simulating on a 16 bit system, start of range invalid");
    if(end >= 16) throw std::invalid_argument("Simulating on a 16 bit system, end of range invalid");
    if(end < start) throw std::invalid_argument("Invalid range, end must be geq start");
    // mask away all bits above end 
    unsigned int mask = ~0x0;
    mask = ~(mask << (end + 1));
    return (target & mask) >> start;
}

uint16_t combine_bytes(uint8_t msb, uint8_t lsb) {
    return (msb << 8) | lsb;
}

/// returns destination from schema 0b000 - 0b100 = Registers B-L, 0b110 = Memory[HL], 0b111 = Reg A
RegisterOpt get_dest8_from_bits(int bits) {
    if(bits > 0b111) throw std::invalid_argument("Bits must be in the range 0b000 - 0b111");
    if(bits == 0b110) return HL;
    return bits == 0b111 ? A : static_cast<Register8>(bits+1);
}

/// 00 -> BC, 01 -> DE, 10 -> HL, 11 -> SP this function always returns a valid 
Register16 get_register16_from_opcode(int opcode) {
    int bits = msb_8(opcode) & 0b11;
    // 00 should be BC, AF is rarely referenced directly as we don't want to set the flag bits
    return static_cast<Register16>(bits + 1);
}

int arith8_mcycles(int opcode) {
    return (opcode & 0b111) == 0b110 ? 2 : 1;
}

int prefixed_ldh_mcycles(int opcode) {
    switch(lsb_8(opcode)) {
        case 0x0: return 3;
        case 0x2: return 2;
        case 0xA: return 4;
    }
    throw std::runtime_error("Invalid argument provided to function prefixed_ldh_mcycles");
}