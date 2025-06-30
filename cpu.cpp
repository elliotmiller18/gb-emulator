#include "cpu.h"
#include "register.h"

int Cpu::execute() {
    int instruction = 0;
    while(true) {
        bool continue_decode = false;
        instruction |= fetch_and_inc();
        // see https://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html
        switch(instruction) {
        // 0 prefix
            //NOP
            case 0x00:
                cycle(4);
                break;
            case 0x01:
                //implementation
            default:
            // misc incomplete instruction
                instruction <<= 8;
                continue_decode = true;
                break;
        }
        if(!continue_decode) instruction = 0;
    }
}

int Cpu::fetch_and_inc() {
    uint16_t old_pc = registers.read(PC);
    registers.write(PC, static_cast<uint16_t>(old_pc+1));
    return memory.read_byte(old_pc);
}