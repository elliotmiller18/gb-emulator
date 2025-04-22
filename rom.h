#ifndef ROM_H
#define ROM_H

#include "utils.h"
#include <vector>

class Rom {
public:

    std::vector<byte>text;
    Rom(const char* filename);
    ~Rom();    
};

#endif
