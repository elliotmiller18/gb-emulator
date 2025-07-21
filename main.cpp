#include "cpu.h"
#include <iostream>
#include <fstream>
#include <handler-table.h>

int main() {
    Cpu cpu("roms/dmg_boot.bin");
    int counter = 0;
    cpu.debug = false;
    while(cpu.current_opcode != 0xA7 && counter < 100000) {
        if(cpu.registers.registers[PC] == 0x64) cpu.debug = true;
        if(cpu.debug) cpu.print_state();
        cpu.current_opcode = cpu.fetch_and_inc();
        if(cpu.debug) {
            std::cout << "\nexecuting instruction number " << std::to_string(counter) << " opcode: " << std::hex << cpu.current_opcode << "\n"; 
        }
        (cpu.*opcode_table[cpu.current_opcode])();
        counter++;
    }
    if(cpu.debug) cpu.print_state();
}

