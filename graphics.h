#pragma once

#include "utils.h"

constexpr auto DOT_NS = SYSTEM_CLOCK_TICKRATE_NS;
constexpr address_t TILE_BASE_ADDR = 8000;

// LCD STATUS BITS (PPU status is 2 bits long)
constexpr int PPU_LSB = 0;
constexpr int PPU_MSB = 1;
constexpr int LYC_EQ_LY_BIT = 2;
constexpr int MODE_0_SELECT_BIT = 3;
constexpr int MODE_1_SELECT_BIT = 4;
constexpr int MODE_2_SELECT_BIT = 5;
constexpr int LYC_INT_SELECT_BIT = 6;

// LCD CONTROL BITS
constexpr int BG_WINDOW_ENABLE_PRIO_BIT = 0;
constexpr int OBJ_ENABLE_BIT = 1;
constexpr int OBJ_SIZE_BIT = 2;
constexpr int BG_TILE_MAP_BIT = 3;
constexpr int BG_WINDOW_TILE_MAP_BIT = 4;
constexpr int WINDOW_ENABLE_BIT = 5;
constexpr int WINDOW_TILE_MAP_BIT = 6;
constexpr int LCD_PPU_ENABLE_BIT = 7;

// OAM ATTRIBUTE BITS
constexpr int PALETTE_BIT = 4; //OAM attribute bit
constexpr int X_FLIP_BIT = 5; //OAM attribute bit
constexpr int Y_FLIP_BIT = 6; //OAM attribute bit
constexpr int PRIORITY_BIT = 7; //OAM attribute bit
 
constexpr int OAM_SIZE = 160;
constexpr auto DOT_SPEED_NS = SYSTEM_CLOCK_TICKRATE_NS;
constexpr int DOTS_PER_OAM_SCAN = 80;
constexpr int DOTS_PER_SCANLINE = 456;
constexpr int DOTS_PER_VBLANK = DOTS_PER_SCANLINE * 10;
constexpr int MINIMUM_MODE_3_TIME = 172;
constexpr int FIRST_VBLANK_SCANLINE = 144;
constexpr int LAST_VBLANK_SCANLINE = 153;

constexpr int OBJECT_X_OFFSET = 8;
constexpr int OBJECT_Y_OFFSET = 16;

constexpr int TILE_WIDTH = 8;

constexpr address_t OAM_BASE_ADDR = 0xFE00;
constexpr address_t OAM_END_ADDR = 0xFE9F;