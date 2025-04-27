#include "rom.h"
#include "utils.h"
#include <fstream>
#include <iostream>

Rom::~Rom() {}

Rom::Rom(const char* filename) {
    std::ifstream rom(filename, std::ios::binary);
    text.resize(std::filesystem::file_size(filename));
    rom.read(reinterpret_cast<char*>(text.data()), text.size());
}
