#include <stack>
#include "cpu.h"
#include "utils.h"

constexpr address_t TILE_BASE_ADDR = 8000;
constexpr int OBJECT_CONTROL_BIT = 1;
constexpr int TILE_SIZE_CONTROL_BIT = 2;    
constexpr int OAM_SIZE = 160;

void Cpu::graphics_handler() {
    //TODO: finish this, this is all graphics behavior per CPU cycle
    uint8_t lcd_status = memory.read_byte(LCD_STATUS_ADDR);
    uint8_t new_lcd_status = lcd_status;
    // sets or resets the lyc to ly comparison bit depneding on whether or not the LYC or LC MMIO addr are the same
    new_lcd_status = set_or_reset_bit(LYC_EQ_LY_BIT, lcd_status, 
        memory.read_byte(LCD_Y_COORDINATE_ADDR) == memory.read_byte(LCD_LY_COMPARE_ADDR)
    );
    //TODO: make this actually reflect the state of the PPU
    bool ppu = false;
}

void Cpu::render() {

}

std::vector<address_t> Cpu::oam_scan_line() {
    int lcd_control = memory.read_byte(LCD_CONTROL_ADDR);
    // if objects are disabled just return an empty vector of objects to draw to the screen
    if(!get_bit(lcd_control, OBJECT_CONTROL_BIT)) return {};
    
    std::vector<address_t> result;
    int objects = 0;
    uint8_t lcd_y = memory.read_byte(LCD_Y_COORDINATE_ADDR);
    int tile_height = get_bit(lcd_control, TILE_SIZE_CONTROL_BIT) ? 16 : 8;
    result.reserve(10);
    //layout is [y coordinate][x coordinate][attributes][padding]
    for(address_t oam_ptr = OAM_BASE_ADDR; oam_ptr < OAM_END_ADDR && objects < 10; oam_ptr += 4) {
        uint8_t starting_y_pos = memory.read_byte(oam_ptr) - 16;
        // if lcd_y is past the end of the tile or before the start of the tile
        if(lcd_y >= starting_y_pos + tile_height || lcd_y < starting_y_pos) continue;
        ++objects;
        result.push_back(oam_ptr);
    }
    return result;
}

void Cpu::oam_dma_transfer(uint8_t source_addr) {
    address_t source_base = combine_bytes(source_addr, 0);
    std::memcpy(
        &memory.memory[OAM_BASE_ADDR],
        &memory.memory[source_base],
        OAM_SIZE
    );
    // 1 for each memcpy
    cycle(OAM_SIZE);
}