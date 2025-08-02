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
    current_opcode = fetch_and_inc();
    if(debug) std::cout << "\nexecuting instruction with opcode: " << std::hex << current_opcode << "\n";
    //TODO: make this a function that returns the number of mycles rather than being a void func
    return (this->*opcode_table[current_opcode])();
}

//TODO: generally refactor this to support the actual non-hacky implementation of having an internal divider register that the timer tracks
void Cpu::run() {
    //TODO: https://gbdev.io/pandocs/Timer_Obscure_Behaviour.html#relation-between-timer-and-divider-register
    //TODO: implement hardware bugs i hate this shitty ass company 😭✌️
    int div_cycles = 0;
    int timer_cycles = 0;
    for(;;) {
        int mcycles = step();
        //TODO: add halt handling
        div_cycles += mcycles;
        timer_cycles += mcycles;
        if(div_cycles >= CYCLES_TO_INC_DIV) {
            div_cycles -= CYCLES_TO_INC_DIV;
            //we don't increment DIV in stop mode but the CPU does keep cycling
            if(!stop_mode) memory.tick_divider();
        }

        uint8_t timer_control = memory.read_byte(TIMER_CONTROL_ADDR);
        // bit 2 of timer_control enables or disables the timer increment
        if(get_bit(timer_control, 2)) {
            int clock_select = get_bits_in_range(timer_control, 0, 1);
            // 00 means 256 cycles to increment, otherwise 0b01 is 4, 0b10 is 16, and 0b11 is 64
            uint8_t cycles_to_inc_timer;
            switch(clock_select) {
                case 0b00: cycles_to_inc_timer = 256; break;
                case 0b01: cycles_to_inc_timer = 4; break;
                case 0b10: cycles_to_inc_timer = 16; break;
                case 0b11: cycles_to_inc_timer = 64; break;
                default: throw std::runtime_error("get_bits_in_range returned an impossible value");
            }
            while(timer_cycles >= cycles_to_inc_timer) {
                //TODO: use result of this to do interrupt
                memory.tick_timer();
                // max possible cycles_to_inc_timer
                timer_cycles -= cycles_to_inc_timer;
            }
        }

        //TODO: interrupts here
        if(debug) {
            if(stop_mode) std::cout << "stopped\n";
            else std::cout << "divider: " << static_cast<int>(memory.read_byte(DIV_ADDR)) << "\n";
            print_state();
        }

        throttle_to_time(mcycles);
    }
}

void Cpu::throttle_to_time(int mycyles) {
    //I do nothing! for now
}