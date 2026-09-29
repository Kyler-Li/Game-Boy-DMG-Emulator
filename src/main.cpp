#include <iostream>
#include <SDL3/SDL.h>
#include "memory.h"

#define TEST_SDL 0

int main(){
    #if TEST_SDL
        // Test the SDL library
        std::cout << "Before SDL init\n";

        if (!SDL_Init(SDL_INIT_VIDEO)) {
            std::cerr << "SDL_Init failed\n";
            return 1;
        }

        std::cout << "SDL init finished, exiting...\n";
        SDL_Quit();
    #endif
    gbdmge::Memory memory;
    memory.loadRom(R"(C:\Users\drago\Desktop\Projects\Game-Boy-DMG-Emulator-\roms\Pokemon Yellow.gb)");
    return 0;
}