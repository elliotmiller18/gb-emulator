#include "cpu.h"
#include <iostream>
#include <fstream>
#include <handler-table.h>

int main() {
    Cpu cpu("roms/dmg_boot.bin");
    cpu.debug = true;
    cpu.run();
}

