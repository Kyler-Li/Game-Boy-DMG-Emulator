#include <SDL3/SDL.h>
#include <iostream>
#include "console.h"

int main(int argc, const char* argv[]) {
  // Test the SDL library
  std::cout << "Before SDL init\n";

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    std::cerr << "SDL_Init failed\n";
    return 1;
  }

  if (argc > 1) {
    const auto romPath = std::filesystem::path{argv[1]};
    auto console = gbdmge::Console{};
    console.load(romPath);
    console.play();
  }

  std::cout << "SDL init finished, exiting...\n";
  SDL_Quit();
  return 0;
}