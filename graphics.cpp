#include "cpu.h"
#include "graphics.h"
#include "utils.h"

struct GraphicsObject {
    uint8_t y;
    uint8_t x;
    uint8_t tile;
    uint8_t attributes;

    GraphicsObject(uint8_t y_pos, uint8_t x_pos, uint8_t tile_num, uint8_t attrs)
        : y(y_pos), x(x_pos), tile(tile_num), attributes(attrs) {}
};

uint8_t Cpu::get_lcd_control() const {
    return memory.read_byte(LCD_CONTROL_ADDR);
}

void Cpu::graphics_handler() {
    //TODO: finish this, this is all graphics behavior per CPU cycle
    uint8_t lcd_status = memory.read_byte(LCD_STAT_ADDR);
    uint8_t new_lcd_status = lcd_status;
    // sets or resets the lyc to ly comparison bit depneding on whether or not the LYC or LC MMIO addr are the same
    new_lcd_status = set_or_reset_bit(LYC_EQ_LY_BIT, lcd_status, 
        memory.read_byte(LCD_LY_ADDR) == memory.read_byte(LCD_LY_COMPARE_ADDR)
    );
    //TODO: make this actually reflect the state of the PPU
    bool ppu = false;
}

//gets a tile from a specific line of a tile
int get_palette_index(int x, uint16_t data) {
    return (get_bit(data, x) << 1) | get_bit(data, x + 7);
}

void Cpu::draw_line() {
    uint8_t lcd_control = get_lcd_control();
    if(!get_bit(lcd_control, LCD_PPU_ENABLE_BIT)) return;

    std::vector<GraphicsObject> object = oam_scan_line();
    // 2 bits per pixel
    scanline_t scanline;
    draw_objects(scanline, object);
}

void Cpu::draw_objects(scanline_t& scanline, std::vector<GraphicsObject>& objects) const {
    // anything below these (unless it's cut off on the other end) 
    // is displayed on the screen somewhat, larger than screen width because of partially 
    // displaying objects
    constexpr int MAX_X = 168;

    uint8_t lcd_control = get_lcd_control();
    if(!get_bit(lcd_control, OBJ_ENABLE_BIT)) return;
    int tile_height = get_bit(lcd_control, OBJ_SIZE_BIT) ? 16 : 8;
    
    // we want to draw the leftmost objects last to maintain proper priority
    std::stable_sort(objects.begin(), objects.end(), [](const GraphicsObject& a, const GraphicsObject& b) {
        return a.x > b.x;
    });

    for(const GraphicsObject& obj : objects) {
        // off screen
        if(obj.x == 0 || obj.x >= MAX_X) continue;
        
        bool y_flip = get_bit(obj.attributes, Y_FLIP_BIT);
        bool x_flip = get_bit(obj.attributes, X_FLIP_BIT);

        // the y_coordinate is starting point + 16, even if it's 8 pixels high
        uint8_t y_tile = memory.read_byte(LCD_LY_ADDR) - (obj.y - OBJECT_Y_OFFSET);
        if(y_tile >= tile_height) throw std::runtime_error("Tried to draw object not overlapping with lcd ly in draw_objects");

        //these are both starting positions
        uint8_t x_pos = obj.x - OBJECT_X_OFFSET;
        uint8_t x_tile = 0;

        if(y_flip) y_tile = tile_height - y_tile - 1;
        if(x_flip) x_tile = TILE_WIDTH - 1;

        // 2 bytes per line
        address_t line_addr = TILE_BASE_ADDR + (obj.tile * 16) + (y_tile * 2);
        uint16_t line_data = memory.read_word(line_addr);
        
        int bit_index = x_pos * 2;
        // going below 0 will overflow way past tile width
        while(x_tile < TILE_WIDTH && bit_index < scanline.size()) {
            
            if(bit_index < 0) {
                bit_index += 2;
                x_tile = x_flip ? x_tile - 1 : x_tile + 1;
                continue;
            }

            int palette_index = get_palette_index(x_tile, line_data);
            // obp = object palette 
            int palette_data = get_bit(obj.attributes, PALETTE_BIT) 
                            ? memory.read_byte(OBP1_ADDR) : memory.read_byte(OBP0_ADDR);
            // palette in memory is [index 3][index 2][index 1][index 0] where each is 2 bits
            int colored_pixel = (palette_data >> (palette_index * 2)) & 0b11;

            bool existing_pixel = scanline[bit_index] != 0 || scanline[bit_index + 1] != 0;
            // we don't draw transparent pixels, 0 in objects means transparent
            if(colored_pixel == 0 || (get_bit(obj.attributes, PRIORITY_BIT) && existing_pixel)) {
                continue;
            }

            scanline[bit_index] = get_bit(colored_pixel, 1);
            scanline[bit_index + 1] = get_bit(colored_pixel, 0);
            bit_index += 2;
            x_tile = x_flip ? x_tile - 1 : x_tile + 1;
        }
    }
}

std::vector<GraphicsObject> Cpu::oam_scan_line() const {
    // if objects are disabled just return an empty vector of objects to draw to the screen
    uint8_t lcd_control = get_lcd_control();
    if(!get_bit(lcd_control, OBJ_ENABLE_BIT)) return {};
    
    std::vector<GraphicsObject> result;
    int objects = 0;
    uint8_t lcd_y = memory.read_byte(LCD_LY_ADDR);
    int tile_height = get_bit(lcd_control, OBJ_SIZE_BIT) ? 16 : 8;
    result.reserve(10);
    //layout is [y coordinate][x coordinate][attributes][padding]
    for(address_t oam_ptr = OAM_BASE_ADDR; oam_ptr < OAM_END_ADDR && objects < 10; oam_ptr += 4) {
        uint8_t starting_y_pos = memory.read_byte(oam_ptr) - OBJECT_Y_OFFSET;
        // if lcd_y is past the end of the tile (because we're 0-indexed we do geq not greater) or before the start of the tile
        if(lcd_y >= starting_y_pos + tile_height || lcd_y < starting_y_pos) continue;
        ++objects;
        // structs are laid out the exact same way that they are in OAM
        result.emplace_back(
            memory.memory[oam_ptr],
            memory.memory[oam_ptr + 1],
            memory.memory[oam_ptr + 2],
            memory.memory[oam_ptr + 3]
        );
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
