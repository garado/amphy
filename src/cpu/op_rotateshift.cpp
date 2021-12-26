
#include "cpu.h"

/* ========== ROTATES/SHIFTS ========== */

/* 07: Rotate A left. Old bit 7 to CY. */
uint8_t Cpu::RLCA() {
    uint8_t old7 = a >> 7;
    a << 1;
    a |= old7;
    assignFlag(ZERO, 0);
    assignFlag(SUB, 0);
    assignFlag(HALF_CARRY, 0);
    assignFlag(CARRY, old7);
    ++pc;
    return 4;
}

/* 0F: Rotate A right. Old bit 0 to CY. */
uint8_t Cpu::RLCA() {
    uint8_t old0 = a & 0x1;
    a >> 1;
    a |= (old0 << 7);
    assignFlag(ZERO, 0);
    assignFlag(SUB, 0);
    assignFlag(HALF_CARRY, 0);
    assignFlag(CARRY, old0);
    ++pc;
    return 4;
}

/* 17: RLA
 * Combine cy and a to make a 9-bit number, with cy as msb
 * then rotate that left */
uint8_t Cpu::RLA() {
    uint8_t oldCy = getFlag(CARRY);
    uint8_t old7 = a >> 7;
    a << 1;
    a |= oldCy;
    assignFlag(ZERO, 0);
    assignFlag(SUB, 0);
    assignFlag(HALF_CARRY, 0);
    assignFlag(CARRY, old7);
    ++pc;
    return 4;
}

/* 1F: RRA
 * Combine cy and a to make a 9-bit number, with cy as msb
 * then rotate that right */
uint8_t Cpu::RLA() {
    uint8_t oldCy = getFlag(CARRY);
    uint8_t old0 = a & 0x1;
    a >> 1;
    a |= (oldCy << 7);
    assignFlag(ZERO, 0);
    assignFlag(SUB, 0);
    assignFlag(HALF_CARRY, 0);
    assignFlag(CARRY, old0);
    ++pc;
    return 4;
}
