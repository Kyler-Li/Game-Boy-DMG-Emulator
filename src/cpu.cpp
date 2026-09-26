#include "cpu.h"

void CPU::reset(){
    /*
    The boom ROM initializes the CPU registers
    to specific values after it completes. Intialize
    the virtual registers to these values.
    */
    regAF.set(0x01, 0xB0);
    regBC.set(0x00, 0x13);
    regDE.set(0x00, 0xD8);
    regHL.set(0x01, 0x4D);
    SP = 0xFFFE;
    PC = 0x0100;
}