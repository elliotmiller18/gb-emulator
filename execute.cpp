#include "cpu.h"
#include "utils.h"
#include "handler-table.h"
#include <iostream>
#include <chrono>
#include <thread>

/// rate (hz) at which the div register is incremented, this is also 1 << 14 or 2^14
constexpr int MASTER_CLOCK_SPEED_HZ = 4'194'304;
// the length of one machine cycle
constexpr double SYSTEM_CLOCK_SPEED_HZ = MASTER_CLOCK_SPEED_HZ / 4;
constexpr double DIV_TICKRATE_HZ = 16384;
constexpr double LCD_REFRESH_RATE_HZ = 59.7;
// 1 second (1 billion ns) divided by the increment rate
constexpr long long SECOND_NS = 1'000'000'000.0;
// note that there is some tiny rounding error here on the subnanosecond level but we can accept that error as 
// simulating subnanosecond time is challenging
constexpr auto DIV_TICKRATE_NS = std::chrono::nanoseconds(static_cast<long long>(SECOND_NS / DIV_TICKRATE_HZ));
constexpr auto SYSTEM_CLOCK_TICKRATE_NS = std::chrono::nanoseconds(static_cast<long long>(SECOND_NS / SYSTEM_CLOCK_SPEED_HZ));
constexpr auto LCD_REFRESH_RATE_NS = std::chrono::nanoseconds(static_cast<long long>(SECOND_NS / LCD_REFRESH_RATE_HZ));

int Cpu::step() {
    current_opcode = fetch_and_inc();
    if(debug) std::cout << "\nexecuting instruction with opcode: " << std::hex << current_opcode << "\n";
    return (this->*opcode_table[current_opcode])();
}

void Cpu::run() {
    //TODO: https://gbdev.io/pandocs/Timer_Obscure_Behaviour.html#relation-between-timer-and-divider-register
    //TODO: implement hardware bugs i hate this shitty ass company 😭✌️
    auto last_div_inc = std::chrono::steady_clock::now();
    auto last_timer_inc = last_div_inc;
    auto last_lcd_refresh = last_div_inc;
    auto lag_time = std::chrono::nanoseconds(0);
    
    for(;;) {
        auto frame_start_time = std::chrono::steady_clock::now();
        
        //TODO: check if we want this before or after interrupts
        poll_input();
        
        int mcycles = 0;
        bool pending_interrupt_flag = pending_interrupt();

        if(halt_mode) halt_mode = pending_interrupt_flag;
        //TODO: implement hardware bug here
        else if(buggy_halt_mode) buggy_halt_mode = pending_interrupt_flag;
        else mcycles = step();

        // ei enables interrupts only after the instruction following it is executed
        if(queued_interrupt_enable && current_opcode != EI_OPCODE) {ime = true; queued_interrupt_enable = false;}

        // for now we just in case we keep the checks that we actually want to run the interrupt
        if(pending_interrupt_flag && check_and_handle_interrupts()) mcycles += 5;

        // ----------------------------- TICK DIV ---------------------------------
        auto current_time = std::chrono::steady_clock::now();
        if(current_time - last_div_inc >= DIV_TICKRATE_NS) {
            last_div_inc += DIV_TICKRATE_NS;
            //we don't increment DIV in stop mode but the CPU does keep cycling
            if(!stop_mode) memory.tick_divider();
            //don't want to increment div a bunch of times after we leave stop mode
            last_div_inc += DIV_TICKRATE_NS;
        }
        // ------------------------------------------------------------------------

        // ----------------------------- TICK TIMER -------------------------------
        uint8_t timer_control = memory.read_byte(TIMER_CONTROL_ADDR);
        // bit 2 of timer_control enables or disables the timer increment
        if(get_bit(timer_control, 2)) {
            int clock_select = get_bits_in_range(timer_control, 0, 1);
            // 00 means 256 cycles to increment, otherwise 0b01 is 4, 0b10 is 16, and 0b11 is 64
            auto timer_rate_ns = SYSTEM_CLOCK_TICKRATE_NS;
            switch(clock_select) {
                case 0b00: timer_rate_ns *= 256; break;
                case 0b01: timer_rate_ns *= 4; break;
                case 0b10: timer_rate_ns *= 16; break;
                case 0b11: timer_rate_ns *= 64; break;
                default: throw std::runtime_error("get_bits_in_range returned an impossible value");
            }
            if(current_time - last_timer_inc >= timer_rate_ns) {
                if(memory.tick_timer()) request_interrupt(TIMER_CONTROL_BIT);
                last_timer_inc += timer_rate_ns;
            }
        }
        // ------------------------------------------------------------------------


        // check if we need to draw a new frame
        if(current_time - last_lcd_refresh > LCD_REFRESH_RATE_NS) {
            last_lcd_refresh += LCD_REFRESH_RATE_NS;
            request_interrupt(LCD_CONTROL_BIT);
        }

        if(debug) print_state();

        auto frame_runtime = std::chrono::steady_clock::now() - frame_start_time;
        // throttle to normal time
        auto time_to_sleep = (SYSTEM_CLOCK_TICKRATE_NS * mcycles) - frame_runtime - lag_time;
        if(time_to_sleep > std::chrono::nanoseconds(0)) {
            std::this_thread::sleep_for(time_to_sleep);
            lag_time = std::chrono::nanoseconds(0);
        } else {
            // if this frame ran for too long (can also accumulate from multiple frames running too long)
            lag_time += frame_runtime - (SYSTEM_CLOCK_TICKRATE_NS * mcycles);
        }
    }
}