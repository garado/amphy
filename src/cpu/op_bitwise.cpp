
#include "cpu.h"

/* ========== BITWISE ========== */
/*  AND n
 *  Logical AND n with A, store result in A. */
uint8_t Cpu::AND_A_A() {
    a &= a;
    assignFlag(ZERO, (a==0));
    assignFlag(SUB, 0);
    assignFlag(HALF_CARRY, 1);
    assignFlag(CARRY, 0);
    ++pc;
    return 4;
}

uint8_t Cpu::AND_A_B() {
    a &= b;
    assignFlag(ZERO, (a==0));
    assignFlag(SUB, 0);
    assignFlag(HALF_CARRY, 1);
    assignFlag(CARRY, 0);
    ++pc;
    return 4;
}

uint8_t Cpu::AND_A_C() {
    a &= c;
    assignFlag(ZERO, (a==0));
    assignFlag(SUB, 0);
    assignFlag(HALF_CARRY, 1);
    assignFlag(CARRY, 0);
    ++pc;
    return 4;
}

uint8_t Cpu::AND_A_D() {
    a &= d;
    assignFlag(ZERO, (a==0));
    assignFlag(SUB, 0);
    assignFlag(HALF_CARRY, 1);
    assignFlag(CARRY, 0);
    ++pc;
    return 4;
}

uint8_t Cpu::AND_A_E() {
    a &= e;
    assignFlag(ZERO, (a==0));
    assignFlag(SUB, 0);
    assignFlag(HALF_CARRY, 1);
    assignFlag(CARRY, 0);
    ++pc;
    return 4;
}

uint8_t Cpu::AND_A_H() {
    a &= h;
    assignFlag(ZERO, (a==0));
    assignFlag(SUB, 0);
    assignFlag(HALF_CARRY, 1);
    assignFlag(CARRY, 0);
    ++pc;
    return 4;
}

uint8_t Cpu::AND_A_L() {
    a &= l;
    assignFlag(ZERO, (a==0));
    assignFlag(SUB, 0);
    assignFlag(HALF_CARRY, 1);
    assignFlag(CARRY, 0);
    ++pc;
    return 4;
}

/*  A6: AND (HL)
 *  Logical AND (HL) with A, store result in A. */
uint8_t Cpu::AND_A_atHL() {
    uint16_t address = (h << 8) | l;
    uint8_t val = bus->read(address);
    a &= val;
    assignFlag(ZERO, (a==0));
    assignFlag(SUB, 0);
    assignFlag(HALF_CARRY, 1);
    assignFlag(CARRY, 0);
    ++pc;
    return 8;
}

/*  AND_A_u8
 *  Logical AND n with A, store result in A. */
uint8_t Cpu::AND_A_u8() {
    uint8_t val = bus->read(++pc);
    a &= val;
    assignFlag(ZERO, (a==0));
    assignFlag(SUB, 0);
    assignFlag(HALF_CARRY, 1);
    assignFlag(CARRY, 0);
    ++pc;
    return 8;
}

/*  OR n
 *  Logical OR n with A, store result in A. */
uint8_t Cpu::OR_A_A() {
    a |= a;
    assignFlag(ZERO, (a==0));
    assignFlag(SUB, 0);
    assignFlag(HALF_CARRY, 0);
    assignFlag(CARRY, 0);
    ++pc;
    return 4;
}

uint8_t Cpu::OR_A_B() {
    a |= b;
    assignFlag(ZERO, (a==0));
    assignFlag(SUB, 0);
    assignFlag(HALF_CARRY, 0);
    assignFlag(CARRY, 0);
    ++pc;
    return 4;
}

uint8_t Cpu::OR_A_C() {
    a |= c;
    assignFlag(ZERO, (a==0));
    assignFlag(SUB, 0);
    assignFlag(HALF_CARRY, 0);
    assignFlag(CARRY, 0);
    ++pc;
    return 4;
}

uint8_t Cpu::OR_A_D() {
    a |= d;
    assignFlag(ZERO, (a==0));
    assignFlag(SUB, 0);
    assignFlag(HALF_CARRY, 0);
    assignFlag(CARRY, 0);
    ++pc;
    return 4;
}

uint8_t Cpu::OR_A_E() {
    a |= e;
    assignFlag(ZERO, (a==0));
    assignFlag(SUB, 0);
    assignFlag(HALF_CARRY, 0);
    assignFlag(CARRY, 0);
    ++pc;
    return 4;
}

uint8_t Cpu::OR_A_H() {
    a |= h;
    assignFlag(ZERO, (a==0));
    assignFlag(SUB, 0);
    assignFlag(HALF_CARRY, 0);
    assignFlag(CARRY, 0);
    ++pc;
    return 4;
}

uint8_t Cpu::OR_A_L() {
    a |= l;
    assignFlag(ZERO, (a==0));
    assignFlag(SUB, 0);
    assignFlag(HALF_CARRY, 0);
    assignFlag(CARRY, 0);
    ++pc;
    return 4;
}

/*  OR (HL)
 *  Logical OR (HL) with A, store result in A. */
uint8_t Cpu::OR_A_atHL() {
    uint16_t address = (h << 8) | l;
    uint8_t val = bus->read(address);
    a |= val;
    assignFlag(ZERO, (a==0));
    assignFlag(SUB, 0);
    assignFlag(HALF_CARRY, 0);
    assignFlag(CARRY, 0);
    ++pc;
    return 8;
}

/*  OR_A_u8
 *  Logical OR n with A, store result in A. */
uint8_t Cpu::OR_A_u8() {
    uint8_t val = bus->read(++pc);
    a |= val;
    assignFlag(ZERO, (a==0));
    assignFlag(SUB, 0);
    assignFlag(HALF_CARRY, 0);
    assignFlag(CARRY, 0);
    ++pc;
    return 8;
}

/*  XOR_A_u8
 *  Logical XOR n with A, store result in A. */
uint8_t Cpu::XOR_A_u8() {
    uint8_t val = bus->read(++pc);
    a ^= val;
    assignFlag(ZERO, (a==0));
    assignFlag(SUB, 0);
    assignFlag(HALF_CARRY, 0);
    assignFlag(CARRY, 0);
    ++pc;
    return 8;
}

/*  XOR n
 *  A ^= n */
uint8_t Cpu::XOR_A_A() {
    a ^= a;
    assignFlag(ZERO, (a==0));
    assignFlag(SUB, 0);
    assignFlag(CARRY, 0);
    assignFlag(HALF_CARRY, 0);
    ++pc;
    return 4;
}

uint8_t Cpu::XOR_A_B() {
    a ^= b;
    assignFlag(ZERO, (b==0));
    assignFlag(SUB, 0);
    assignFlag(CARRY, 0);
    assignFlag(HALF_CARRY, 0);
    ++pc;
    return 4;
}

uint8_t Cpu::XOR_A_C() {
    a ^= c;
    assignFlag(ZERO, (c==0));
    assignFlag(SUB, 0);
    assignFlag(CARRY, 0);
    assignFlag(HALF_CARRY, 0);
    ++pc;
    return 4;
}

uint8_t Cpu::XOR_A_D() {
    a ^= d;
    assignFlag(ZERO, (d==0));
    assignFlag(SUB, 0);
    assignFlag(CARRY, 0);
    assignFlag(HALF_CARRY, 0);
    ++pc;
    return 4;
}

uint8_t Cpu::XOR_A_E() {
    a ^= e;
    assignFlag(ZERO, (e==0));
    assignFlag(SUB, 0);
    assignFlag(CARRY, 0);
    assignFlag(HALF_CARRY, 0);
    ++pc;
    return 4;
}

uint8_t Cpu::XOR_A_H() {
    a ^= h;
    assignFlag(ZERO, (h==0));
    assignFlag(SUB, 0);
    assignFlag(CARRY, 0);
    assignFlag(HALF_CARRY, 0);
    ++pc;
    return 4;
}

uint8_t Cpu::XOR_A_L() {
    a ^= l;
    assignFlag(ZERO, (l==0));
    assignFlag(SUB, 0);
    assignFlag(CARRY, 0);
    assignFlag(HALF_CARRY, 0);
    ++pc;
    return 4;
}

/*  XOR (HL)
 *  Logical XOR (HL) with A, store result in A. */
uint8_t Cpu::XOR_A_atHL() {
    uint16_t address = (h << 8) | l;
    uint8_t val = bus->read(address);
    a ^= val;
    assignFlag(ZERO, (a==0));
    assignFlag(SUB, 0);
    assignFlag(HALF_CARRY, 0);
    assignFlag(CARRY, 0);
    ++pc;
    return 8;
}

uint8_t Cpu::XOR_A_u8() {
    uint16_t address = (h << 8) | l;
    uint8_t val = bus->read(address);
    a ^= val;
    assignFlag(ZERO, (a==0));
    assignFlag(SUB, 0);
    assignFlag(HALF_CARRY, 0);
    assignFlag(CARRY, 0);
    ++pc;
    return 8;
}