#include "cpu.h"
#include <iostream>
#include <fstream>
#include <handler-table.h>

int main() {
    Cpu cpu("roms/dmg_boot.bin");
    int counter = 0;
    cpu.debug = true;
    while(counter < 100000) {
        cpu.print_state();
        cpu.current_opcode = cpu.fetch_and_inc();
        std::cout << "executing " << std::hex << cpu.current_opcode; 
        (cpu.*opcode_table[cpu.current_opcode])();
        std::cout << "\n";
        counter++;
    }
    cpu.print_state();
}

