# Game-Boy-DMG-Emulator-
A GameBoy emulator for our Drexel Senior capstone project

# Build Instructions

Open a terminal in the project root directory.

## Building

First build:

```
cmake -S . -B build
cmake --build build
```

Subsequent builds:

```
cmake --build build
```

## Cleaning

Clean compiled files:

```
cmake --build build --target clean
```

To fully rebuild, delete the `build/` directory and run the first-build commands again.

# Running The Game Boy Emulator

After the build process is completed, run `gbdmge` in the build folder from a terminal.
