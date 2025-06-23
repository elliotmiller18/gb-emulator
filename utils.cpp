#include "utils.h"

uint8_t msb(uint16_t val) {
    return val >> 8;
}

uint8_t lsb(uint16_t val) {
    return static_cast<uint8_t>(val);
}