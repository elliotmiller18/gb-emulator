#include "cpu.h"
#include <SDL2/SDL.h>
#include <unordered_map>
#include "utils.h"

constexpr uint8_t JOYPAD_ADDR = 0xFF00;
constexpr uint8_t SELECT_BUTTONS_BIT = 5;
constexpr uint8_t SELECT_DPAD_BIT = 4; 

enum class JoypadButtons {
    A,
    B,
    Select,
    Start,
    Right,
    Left,
    Up,
    Down,
};

constexpr JoypadButtons select_buttons_joypad_buttons[] = {JoypadButtons::A, JoypadButtons::B, JoypadButtons::Select, JoypadButtons::Start};
constexpr JoypadButtons select_dpad_joypad_buttons[] = {JoypadButtons::Right, JoypadButtons::Left, JoypadButtons::Up, JoypadButtons::Down};

uint8_t get_button_bit(JoypadButtons button) {
    // 4 button input bits * 2 for the two modes
    return static_cast<uint8_t>(button) % 4;
}


static std::unordered_map<JoypadButtons, SDL_Scancode> control_map = {
    // Action Buttons
    {JoypadButtons::A,       SDL_SCANCODE_Z},
    {JoypadButtons::B,       SDL_SCANCODE_X},
    // Game Control Buttons
    {JoypadButtons::Select,  SDL_SCANCODE_C},
    {JoypadButtons::Start,   SDL_SCANCODE_V},
    // Directional Pad (D-pad)
    {JoypadButtons::Right,   SDL_SCANCODE_RIGHT},
    {JoypadButtons::Left,    SDL_SCANCODE_LEFT},
    {JoypadButtons::Up,      SDL_SCANCODE_UP},
    {JoypadButtons::Down,    SDL_SCANCODE_DOWN},
};

uint8_t calculate_input_state(const uint8_t* keystates, const JoypadButtons buttons[]) {
    // we set this to be FF because we will be &ing with it later and only want to set the bottom 4 bits,
    // the lower nibble is assumed to be 1111
    uint8_t raw = 0xFF;
    for(int i = 0; i < 4; i++) {
        // set or reset bit depending on keystate
        set_or_reset_bit(get_button_bit(buttons[i]), raw, !keystates[control_map[buttons[i]]]);
    }
    return raw;
}

void Cpu::poll_input() {
    uint8_t joypad_register = memory.memory[JOYPAD_ADDR]; 
    const uint8_t* keystates = SDL_GetKeyboardState(NULL);
    // 0 is true, 1 is false for this
    if(!get_bit(joypad_register, SELECT_BUTTONS_BIT)) joypad_register &= calculate_input_state(keystates, select_buttons_joypad_buttons);
    if(!get_bit(joypad_register, SELECT_DPAD_BIT)) joypad_register &= calculate_input_state(keystates, select_dpad_joypad_buttons);
    memory.memory[JOYPAD_ADDR] = joypad_register;
}