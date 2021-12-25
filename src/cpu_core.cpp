
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "cpu.h"

/*  Cpu::resetFlag()
 *  Flag = 0. */
void Cpu::resetFlag(Flags flag) {
    f &= ~(1 << flag);  // set bit to 0
}

/*  Cpu::setFlag()
 *  Flag = 1. */
void Cpu::setFlag(Flags flag) {
    f |= (1 << flag);   // set bit to 1
}

uint8_t Cpu::getFlag(Flags flag) {
    return (f >> flag) & 0x1;
}

void Cpu::checkHalfCarry(uint8_t a, uint8_t b) {
    if ( (a & 0xF) + (b & 0xF) > 0xF) {
        setFlag(HALF_CARRY);
    } else {
        resetFlag(HALF_CARRY);
    }
}

void Cpu::checkCarry(uint8_t a, uint8_t b) {
    if ( (int) (a + b) > 255) {
        setFlag(CARRY);
    } else {
        resetFlag(CARRY);
    }
}

void Cpu::regdump() {
    std::cout << "af: " << std::hex << (int) a << (int) f << std::endl;
    std::cout << "bc: " << std::hex << (int) b << (int) c << std::endl;
    std::cout << "de: " << std::hex << (int) d << (int) e << std::endl;
    std::cout << "hl: " << std::hex << (int) h << (int) l << std::endl;
    std::cout << "sp: " << std::hex << (int) sp << std::endl;
    std::cout << "pc: " << std::hex << (int) pc << std::endl;
    std::cout << "OP: " << std::hex << (int) bus->read(pc) << std::endl;

    std::cout << "LY: " << std::hex << (int) bus->read(0xFF44) << std::endl;
}

/* temp function
 * 
 */
void Cpu::unknown(uint8_t opcode) {
    std::cout << "Unknown opcode found: " << std::hex << (int) opcode << std::endl;
    regdump();
}
