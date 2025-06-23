#include "cpu.h"

void Cpu::call(uint16_t faddr, bool condition = false) {
    if(!condition || registers.get_flag(z)) {
        push(PC);
        registers.write(PC, faddr);
    }
}
void Cpu::ret(bool condition = false, bool interrupt = false) {
    if(!condition || registers.get_flag(z)) {
        pop(PC);
    }
    if(interrupt) registers.enable_interrupts();
}

