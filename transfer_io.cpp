#include "cpu.h"
#include "memory.h"
#include "register.h"
#include <stdexcept>

uint16_t Cpu::load_to_reg16(Register16 dest, BinOpt16 arg) {
    if(std::holds_alternative<uint16_t>(arg)) {
        if(dest == AF || dest == PC) throw std::logic_error("Cannot load to PC or AF using this instruction");
    } else {
        if(dest != SP) throw std::logic_error("Must load to SP using this instruction");
    }

    registers.write(dest, registers.unpack_binopt16(arg));
    return registers.read(dest);
}

uint16_t Cpu::push(Register16 arg){
    uint16_t sp_value = registers.read(SP) - 2;
    memory.write_word(sp_value, arg);
    return registers.write(SP, sp_value);
}

uint16_t Cpu::pop(Register16 dest){
    uint16_t sp_value = registers.read(SP);
    uint16_t word = memory.read_word(sp_value);
    registers.write(dest, word);
    return registers.write(SP, static_cast<uint16_t>(sp_value + 2));
}