
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "cpu.h"

/* Cpu::assignFlag
 * Assign an f-register flag to a value. */
void Cpu::assignFlag(CpuFlags flag, bool val) {
    f &= ~(1 << flag);      // set bit to 0
    f |= (val << flag);     // so we can set to val
}

bool Cpu::getFlag(CpuFlags flag) {
    return (f >> flag) & 0x1;
}

/* Cpu::setHalfCarryAdd
 * Set half carry flag for addition. */
void Cpu::setHalfCarryAdd(uint8_t a, uint8_t b) {
    bool val = (a & 0xF) + (b & 0xF) > 0xF);
    assignFlag(HALF_CARRY, val);
}

/* Cpu::setCarryAdd
 * Set carry flag for addition. */
void Cpu::setCarryAdd(uint8_t a, uint8_t b) {
    bool val =  (int) (a + b) > 255;
    assignFlag(CARRY, val);
}

/* Cpu::setHalfCarrySub
 * Set half carry flag for subtraction. */
void Cpu::setHalfCarrySub(uint8_t a, uint8_t b) {
    bool val = (a & 0xF) + (b & 0xF) > 0xF);
    assignFlag(HALF_CARRY, val);
}

/* Cpu::setCarrySub
 * Set carry flag for subtraction. */
void Cpu::setCarrySub(uint8_t a, uint8_t b) {
    bool val =  (int) (a + b) > 255;
    assignFlag(CARRY, val);
}

/*  Cpu::setAddFlags
 *  Sets zero, sub, half carry, and carry flags for addition */
void Cpu::setAddFlags(uint8_t a, uint8_t b) {
    uint8_t result = a + b;
    assignFlag(ZERO, (result==0));
    assignFlag(SUB, 0);
    setHalfCarryAdd(a, b);
    setCarryAdd(a, b);
}

/*  Cpu::setSubFlags
 *  Sets zero, sub, half carry, and carry flags for subtraction */
void Cpu::setSubFlags(uint8_t a, uint8_t b) {
    uint8_t result = a - b;
    assignFlag(ZERO, (result==0));
    assignFlag(SUB, 1);
    setHalfCarrySub(a, b);
    setCarrySub(a, b);
}

/* Cpu::setHalfCarryAdd
 * Set half carry flag for addition. */
void Cpu::setHalfCarryAdd(uint16_t a, uint16_t b) {
    bool val = (a & 0xF) + (b & 0xF) > 0xF);
    assignFlag(HALF_CARRY, val);
}

/* Cpu::setCarryAdd
 * Set carry flag for addition. */
void Cpu::setCarryAdd(uint16_t a, uint16_t b) {
    bool val =  (int) (a + b) > 255;
    assignFlag(CARRY, val);
}

/* Cpu::setHalfCarrySub
 * Set half carry flag for subtraction. */
void Cpu::setHalfCarrySub(uint16_t a, uint16_t b) {
    bool val = (a & 0xF) + (b & 0xF) > 0xF);
    assignFlag(HALF_CARRY, val);
}

/* Cpu::setCarrySub
 * Set carry flag for subtraction. */
void Cpu::setCarrySub(uint16_t a, uint16_t b) {
    bool val =  (int) (a + b) > 255;
    assignFlag(CARRY, val);
}

/*  Cpu::setAddFlags
 *  Sets zero, sub, half carry, and carry flags for addition */
void Cpu::setAddFlags(uint16_t a, uint16_t b) {
    uint16_t result = a + b;
    assignFlag(ZERO, (result==0));
    assignFlag(SUB, 0);
    setHalfCarryAdd(a, b);
    setCarryAdd(a, b);
}

/*  Cpu::setSubFlags
 *  Sets zero, sub, half carry, and carry flags for subtraction */
void Cpu::setSubFlags(uint16_t a, uint16_t b) {
    uint16_t result = a - b;
    assignFlag(ZERO, (result==0));
    assignFlag(SUB, 1);
    setHalfCarrySub(a, b);
    setCarrySub(a, b);
}

void Cpu::regdump() {
    std::cout << "af: " << std::hex << (int) a << (int) f << std::endl;
    std::cout << "bc: " << std::hex << (int) b << (int) c << std::endl;
    std::cout << "de: " << std::hex << (int) d << (int) e << std::endl;
    std::cout << "hl: " << std::hex << (int) h << (int) l << std::endl;
    std::cout << "sp: " << std::hex << (int) sp << std::endl;
    std::cout << "pc: " << std::hex << (int) pc << std::endl;
    std::cout << "OP: " << std::hex << (int) bus->read(pc) << std::endl;
}

/* Cpu::execute
 * Handles execution of all opcodes */
bool Cpu::execute() {
    if (!cpuEnabled) return EXIT_SUCCESS;
    uint8_t op = bus->read(pc);
    uint8_t cyclesElapsed = (this->*opcode[op])();
    
    // janky di instruction implementation
    if (disableInterrupts != 0) {
        --disableInterrupts;
        if (disableInterrupts == 0) {
            interruptMasterEnable = 0;
        }
    }

    // janky ei instruction implementation
    if (enableInterrupts != 0) {
        --enableInterrupts;
        if (enableInterrupts == 0) {
            interruptMasterEnable = 1;
        }
    }

    return EXIT_SUCCESS;
};
