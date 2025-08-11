#include "memory.h"
#include "register.h"
#include "utils.h"
#include "cpu.h"
#include <iostream>
#include <fstream>
#include <variant>

Memory::Memory(Registers& registers, const char* filename, Cpu& cpu)
  : registers(registers), cpu(cpu)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        throw std::runtime_error(std::string("Failed to open ROM: ") + filename);
    }

    auto size = file.tellg();
    if (size < 0 || static_cast<std::size_t>(size) > memory.size()) {
        throw std::runtime_error("ROM size is invalid or too large for memory");
    }

    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(memory.data()), size);
    if (!file) {
        throw std::runtime_error("Error while reading ROM into memory");
    }
}

Memory::~Memory() {}

uint16_t Memory::unpack_addr(BinOpt addr) {
    if(BinOpt16* unpacked = std::get_if<BinOpt16>(&addr)) {
        return registers.unpack_binopt16(*unpacked);
    } else {
        return static_cast<uint16_t>(registers.unpack_binopt8(std::get<BinOpt8>(addr))) | EIGHT_BIT_MEM_PREFIX;
    }
}

uint16_t Memory::read_word_and_inc_sp() {
    uint16_t data = read_word(registers.read(SP));
    registers.write(SP, static_cast<uint16_t>(registers.read(SP) + 2));
    return data;
}

uint16_t Memory::read_word(BinOpt addr) {
    uint16_t unpacked_addr = unpack_addr(addr);

    uint16_t lower_byte = read_byte(unpacked_addr);
    uint16_t upper_byte = read_byte(++unpacked_addr);

    return (upper_byte << 8) | lower_byte;
}

uint8_t Memory::read_byte(BinOpt addr) {
    uint16_t address = unpack_addr(addr);
    if(address == NULL_ADDR) throw std::runtime_error("Trying to read null address");
    return memory[address];
}

void Memory::write_word(BinOpt addr, BinOpt16 val) {
    uint16_t unpacked_value = registers.unpack_binopt16(val);
    uint16_t unpacked_addr = unpack_addr(addr);

    uint8_t msb = msb_16(unpacked_value);
    uint8_t lsb = lsb_16(unpacked_value);

    write_byte(unpacked_addr, lsb);
    write_byte(++unpacked_addr, msb);
}

/// decrements, then writes
void Memory::write_word_and_dec_sp(BinOpt16 val) {
    registers.write(SP, static_cast<uint16_t>(registers.read(SP) - 2));
    write_word(registers.read(SP), val);
}

void Memory::write_byte(BinOpt addr, BinOpt8 val) {
    uint8_t unpacked_value = registers.unpack_binopt8(val);
    uint16_t unpacked_addr = unpack_addr(addr);
    if(unpacked_addr == NULL_ADDR) throw std::runtime_error("Trying to write to null address");
    // writing to div resets it
    if(unpacked_addr == DIV_ADDR) unpacked_value = 0;
    if(unpacked_addr == BOOT_ROM_MAPPING_CONTORL_ADDR) cpu.booting = false;
    if(unpacked_addr == OAM_DMA_ADDR) cpu.oam_dma_transfer(unpacked_value);

    memory[unpacked_addr] = unpacked_value;
}

void Memory::tick_divider() {
    ++memory[DIV_ADDR];
}

bool Memory::tick_timer() {
    //TODO: when we add cycle by cycle simulation, we need to support caching the old value of timer modulo if it's written on the same
    //cycle as ticking
    if(memory[TIMER_COUNTER_ADDR] == 0xFF) {
        memory[TIMER_COUNTER_ADDR] = memory[TIMER_MODULO_ADDR];
        return true;
    }
    ++memory[TIMER_COUNTER_ADDR];
    return false;
}