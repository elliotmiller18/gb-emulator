#include "cpu.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <handler-table.h>
#include <SDL.h>

int main() {
    // init SDL for input
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "GB Emulator",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    Cpu cpu("roms/dmg_boot.bin");
    cpu.debug = true;
    cpu.boot();
    cpu.run();

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
    // cpu.run();
}

