#include "cpu.h"
#include "utils.h"

// Interrupt handler addresses
constexpr int VBLANK_IHANDLER = 0x40;
constexpr int LCD_IHANDLER = 0x48;
constexpr int STAT_IHANDLER = LCD_IHANDLER;
constexpr int TIMER_IHANDLER = 0x50;
// unused for the forseeable future 
constexpr int SERIAL_IHANDLER = 0x58;
constexpr int JOYPAD_IHANDLER = 0x60;



bool Cpu::check_and_handle_interrupts() {
    if(!ime) return false;
    uint8_t interrupt_requested = memory.read_byte(INTERRUPT_FLAG_ADDR); 
    // if no interrupts are requested (high bits are unused, bits 0-4 correspond to interrupts)
    if(get_bits_in_range(interrupt_requested, 0, 4) == 0) return false;

    uint8_t interrupt_enable = memory.read_byte(INTERRUPT_ENABLE_ADDR);
    // interrupt priority goes in reverse order, meaning that vblank has top prio and joypad has lowest
    uint16_t ihandler_addr = 0;
    uint8_t control_bit = 0;
    if(get_bit(interrupt_requested, VBLANK_INTERRUPT_CONTROL_BIT) && get_bit(interrupt_enable, VBLANK_INTERRUPT_CONTROL_BIT)) {
        ihandler_addr = VBLANK_IHANDLER;
        control_bit = VBLANK_INTERRUPT_CONTROL_BIT;
    } else if(get_bit(interrupt_requested, LCD_INTERRUPT_CONTROL_BIT) && get_bit(interrupt_enable, LCD_INTERRUPT_CONTROL_BIT)) {
        // lcd interrupt corresponds to the stat interrupt, idk it's named like this
        ihandler_addr = STAT_IHANDLER;
        control_bit = LCD_INTERRUPT_CONTROL_BIT;
    } else if(get_bit(interrupt_requested, TIMER_INTERRUPT_CONTROL_BIT) && get_bit(interrupt_enable, TIMER_INTERRUPT_CONTROL_BIT)) {
        ihandler_addr = TIMER_IHANDLER;
        control_bit = TIMER_INTERRUPT_CONTROL_BIT;
    } else if(get_bit(interrupt_requested, SERIAL_INTERRUPT_CONTROL_BIT) && get_bit(interrupt_enable, SERIAL_INTERRUPT_CONTROL_BIT)) {
        ihandler_addr = SERIAL_IHANDLER;
        control_bit = SERIAL_INTERRUPT_CONTROL_BIT;
    } else if(get_bit(interrupt_requested, JOYPAD_INTERRUPT_CONTROL_BIT) && get_bit(interrupt_enable, JOYPAD_INTERRUPT_CONTROL_BIT)) {
        ihandler_addr = JOYPAD_IHANDLER;
        control_bit = JOYPAD_INTERRUPT_CONTROL_BIT;
    }

    if(ihandler_addr == 0) return false;

    ime = false;
    // mark interrupt as serviced
    reset_bit(interrupt_requested, control_bit);
    memory.write_byte(INTERRUPT_FLAG_ADDR, interrupt_requested);
    // call interrupt handler
    memory.write_word_and_dec_sp(PC);
    registers.write(PC, ihandler_addr);

    return true;
}

void Cpu::request_interrupt(uint8_t control_bit) {
    uint8_t interrupt_request = memory.read_byte(INTERRUPT_FLAG_ADDR);
    set_bit(interrupt_request, control_bit);
    memory.write_byte(INTERRUPT_FLAG_ADDR, interrupt_request);
}