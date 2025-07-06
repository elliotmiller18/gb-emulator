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

uint8_t Cpu::get_current_opcode() {
    // THIS WILL RELIABLY NEVER BE CALLED DIRECTLY AFTER A JUMP EXECUTES, OR WHEN PC == 0
    // still we have a sanity check here that I'll remove when I'm optimizing everyting
    if(registers.read(PC) == 0) throw std::logic_error("Trying to fetch opcode with PC already at 0");
    return memory.read_byte(static_cast<uint16_t>(registers.read(PC) - 1));
}

void Cpu::print_state() {
    for(size_t r = 0; r < NUM_REGISTERS; r++) {
        std::cout << "REGISTER " << r16_name(r) << ": 0x" << std::hex << registers.read(static_cast<Register16>(r)) << "\n";
    }
}

// BIT OPERATIONS

int msb(uint16_t val) {
    return val >> 8;
}

int lsb(uint16_t val) {
    return static_cast<uint8_t>(val);
}


bool get_bit(int target, int position) {
    return (target >> position) % 2 == 1;
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

/// returns either the register8 that the bits correspond to or HL if it is meant to be a memory read/write
RegisterOpt get_dest8_from_opcode_bits(int bits) {
    if(bits > 0b111) throw std::invalid_argument("Bits must be in the range 0b000 - 0b111");
    if(bits == 0b110) return HL;
    return bits == 0b111 ? A : static_cast<Register8>(bits+1);
}

/// 00 -> BC, 01 -> DE, 10 -> HL, 11 -> SP
Register16 get_register16_from_opcode_bits(int bits) {
    // 00 should be BC, as AF is never referenced directly as we don't want to set the flag bits
    Register16 dest = static_cast<Register16>(bits + 1);
    if(dest == AF || dest == PC) throw std::invalid_argument("Must be within the range 0b00 (BC) - 0b11 (SP)");
    return dest;
}