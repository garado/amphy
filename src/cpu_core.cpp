
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

/* Cpu::setHalfCarryAdd
 * Set half carry flag for addition. */
void Cpu::setHalfCarryAdd(uint8_t a, uint8_t b) {
    if ( (a & 0xF) + (b & 0xF) > 0xF) {
        setFlag(HALF_CARRY);
    } else {
        resetFlag(HALF_CARRY);
    }
}

/* Cpu::setCarryAdd
 * Set carry flag for addition. */
void Cpu::setCarryAdd(uint8_t a, uint8_t b) {
    if ( (int) (a + b) > 255) {
        setFlag(CARRY);
    } else {
        resetFlag(CARRY);
    }
}

/* Cpu::setHalfCarrySub
 * Set half carry flag for subtraction. */
void Cpu::setHalfCarrySub(uint8_t a, uint8_t b) {
    if ( (a & 0xF) + (b & 0xF) > 0xF) {
        setFlag(HALF_CARRY);
    } else {
        resetFlag(HALF_CARRY);
    }
}

/* Cpu::setCarrySub
 * Set carry flag for subtraction. */
void Cpu::setCarrySub(uint8_t a, uint8_t b) {
    if ( (int) (a + b) > 255) {
        setFlag(CARRY);
    } else {
        resetFlag(CARRY);
    }
}

/*  Cpu::setAddFlags
 *  Sets zero, sub, half carry, and carry flags for addition */
void Cpu::setAddFlags(uint8_t a, uint8_t b) {
    uint8_t result = a + b;
    if (result == 0) {
        setFlag(ZERO);
    } else {
        resetFlag(ZERO);
    }
    resetFlag(SUB);
    setHalfCarryAdd(a, b);
    setCarryAdd(a, b);
}

/*  Cpu::setSubFlags
 *  Sets zero, sub, half carry, and carry flags for subtraction */
void Cpu::setSubFlags(uint8_t a, uint8_t b) {
    uint8_t result = a - b;
    if (result == 0) {
        setFlag(ZERO);
    } else {
        resetFlag(ZERO);
    }
    setFlag(SUB);
    if ( (a & 0xF) + (b & 0xF) > 0xF) {
        setFlag(HALF_CARRY);
    } else {
        resetFlag(HALF_CARRY);
    }
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
