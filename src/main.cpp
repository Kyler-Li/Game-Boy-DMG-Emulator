#include <SDL3/SDL.h>
#include <iostream>

int main() {
  // Test the SDL library
  std::cout << "Before SDL init\n";

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    std::cerr << "SDL_Init failed\n";
    return 1;
  }

  std::cout << "SDL init finished, exiting...\n";
  SDL_Quit();
  return 0;
}