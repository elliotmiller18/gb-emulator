#ifndef ROM_H
#define ROM_H

#include <vector>

// total size of physical memory in bytes
constexpr int MEMORY_SIZE = 0xFFFF;
// the starting position of the non boot rom user code
constexpr int USER_CODE_STARTING_ADDR = 0x0100;

class Rom {
public:

    std::vector<uint8_t> text;
    Rom(const char* filename);
    ~Rom();    
};

#endif
