#pragma once

#include <vector>

class Rom {
public:
    std::vector<uint8_t> text;
    Rom(const char* filename);
    ~Rom();    
};

