#include <iostream>
#include "Console.h"

int main(const int argc, const char* argv[]) {
  if (argc <= 1) {
    std::cerr << "Usage: ./gbdmge <cartridge>\n";
    return 1;
  }

  const auto romPath = std::filesystem::path{argv[1]};
  auto console = gbdmge::Console{};

  if (!console.load(romPath)) {
    return 1;
  }

  while (true) {
    console.tick();
  }

  return 0;
}
