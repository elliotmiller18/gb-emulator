#include "utils.h"

int msb(uint16_t val) {
    return val >> 8;
}

int lsb(uint16_t val) {
    return static_cast<uint8_t>(val);
}