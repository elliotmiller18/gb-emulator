#ifndef ROM_H
#define ROM_H

#include <vector>

class Rom {
public:

    std::vector<uint8_t> text;
    Rom(const char* filename);
    ~Rom();    
};

#endif
