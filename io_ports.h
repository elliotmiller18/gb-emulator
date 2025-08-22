#include <cstdint>
// for some reason I didn't think to do this until months into development, so there are a lot of 
// uint16_t's where there should be address_t, sorry 
using address_t = uint16_t;

constexpr address_t JOYPAD_ADDR = 0xFF00;
constexpr address_t DIV_ADDR = 0xFF04;
constexpr address_t TIMER_COUNTER_ADDR = 0xFF05;
constexpr address_t TIMER_MODULO_ADDR = 0xFF06;
constexpr address_t TIMER_CONTROL_ADDR = 0xFF07;

constexpr address_t LCD_CONTROL_ADDR = 0xFF40;
constexpr address_t LCD_STAT_ADDR = 0xFF41;
constexpr address_t SCY_ADDR = 0xFF42;
constexpr address_t SCX_ADDR = 0xFF43;
constexpr address_t LCD_LY_ADDR = 0xFF44;
constexpr address_t LCD_LY_COMPARE_ADDR = 0xFF45;
constexpr address_t OAM_DMA_ADDR = 0xFF46;

constexpr address_t OBP0_ADDR = 0xFF48;
constexpr address_t OBP1_ADDR = 0xFF49;

constexpr address_t BOOT_ROM_MAPPING_CONTORL_ADDR = 0xFF50;

constexpr address_t INTERRUPT_FLAG_ADDR = 0xFF0F;
constexpr address_t INTERRUPT_ENABLE_ADDR = 0xFFFF;