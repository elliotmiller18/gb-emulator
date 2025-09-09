#include "cpu.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <handler-table.h>

int main() {
    // init SDL for input
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    Cpu cpu("roms/dmg_boot.bin");
    cpu.window = SDL_CreateWindow(
        "GB Emulator",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    ); 

    cpu.debug = true;
    cpu.boot();
    cpu.run();

    SDL_DestroyWindow(cpu.window);
    SDL_Quit();
    return 0;
    // cpu.run();
}

