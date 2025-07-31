#include "cpu.h"
#include "utils.h"
#include "handler-table.h"
#include <iostream>

/// rate (hz) at which the div register is incremented, this is also 1 << 14 or 2^14
constexpr int MASTER_CLOCK_SPEED = 4'194'304;
// the length of one machine cycle
constexpr int SYSTEM_CLOCK_SPEED = MASTER_CLOCK_SPEED / 4;
constexpr int DIV_INC_RATE = 16384;
constexpr int CYCLES_TO_INC_DIV = SYSTEM_CLOCK_SPEED / DIV_INC_RATE;

int Cpu::step() {
    if(debug) std::cout << "\nexecuting instruction with opcode: " << std::hex << current_opcode << "\n";
    current_opcode = fetch_and_inc();
    //TODO: make this a function that returns the number of mycles rather than being a void func
    return (this->*opcode_table[current_opcode])();
}

void Cpu::run() {
    //TODO: https://gbdev.io/pandocs/Timer_Obscure_Behaviour.html#relation-between-timer-and-divider-register
    //TODO: implement hardware bugs i hate this shitty ass company 😭✌️
    int div_cycles = 0;
    int timer_cycles = 0;
    // note: In this function we %= under the assumption that mcycles (the result of step) will 
    // never exceed 64 (or 256 but not exceeding 64 obviously implies it won't exceed 256)
    // which we can assume as step is only the result of one instruction which I believe is at most 5 machine cycles
    while(true) {
        int mcycles = step();
        //TODO: add halt handling

        div_cycles += mcycles;
        if(div_cycles >= CYCLES_TO_INC_DIV) {
            div_cycles %= CYCLES_TO_INC_DIV;
            //we don't increment DIV in stop mode but the CPU does keep cycling
            if(!stop_mode) memory.adjust(DIV_ADDR, 1);
        }

        uint8_t timer_control = memory.read_byte(TIMER_CONTROL_ADDR);
        // bit 2 of timer_control enables or disables the timer increment
        if(get_bit(timer_control, 2)) {
            int clock_select = get_bits_in_range(timer_control, 0, 2);
            // 00 means 256 cycles to increment, otherwise 0b01 is 4, 0b10 is 16, and 0b11 is 64
            uint8_t cycles_to_inc_timer;
            switch(clock_select) {
                case 0b00: cycles_to_inc_timer = 256; break;
                case 0b01: cycles_to_inc_timer = 4; break;
                case 0b10: cycles_to_inc_timer = 16; break;
                case 0b11: cycles_to_inc_timer = 64; break;
                default: throw std::runtime_error("get_bits_in_range returned an impossible value");
            }
            if(timer_cycles >= cycles_to_inc_timer) {
                //TODO: use result of this to do interrupt
                memory.tick_timer();
                // max possible cycles_to_inc_timer
                timer_cycles %= 256;
            }
        }

        //TODO:: interrupts here
        if(debug) {
            std::cout << "divider: " << std::hex << memory.read_byte(DIV_ADDR) << "\n";
            print_state();
        }

        throttle_to_time(mcycles);
    }
}

void Cpu::throttle_to_time(int mycyles) {
    //I do nothing!
}