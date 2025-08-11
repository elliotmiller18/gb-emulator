#pragma once

#include "utils.h"

constexpr auto DOT_NS = SYSTEM_CLOCK_TICKRATE_NS;

// PPU status is 2 bits long
constexpr int PPU_LSB = 0;
constexpr int PPU_MSB = 1;
constexpr int LYC_EQ_LY_BIT = 2;
constexpr int MODE_0_SELECT_BIT = 3;
constexpr int MODE_1_SELECT_BIT = 4;
constexpr int MODE_2_SELECT_BIT = 5;
constexpr int LYC_INT_SELECT_BIT = 6;

constexpr address_t OAM_BASE_ADDR = 0xFE00;
constexpr address_t OAM_END_ADDR = 0xFE9F;