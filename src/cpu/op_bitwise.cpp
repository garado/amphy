
#include "cpu.h"

/* ========== BITWISE ========== */
/*  AND n
 *  Logical AND n with A, store result in A. */
uint8_t Cpu::AND_A_A() {
  a &= a;
  AssignFlag(ZERO, (a==0));
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 1);
  AssignFlag(CARRY, 0);
  ++pc;
  return 4;
}

uint8_t Cpu::AND_A_B() {
  a &= b;
  AssignFlag(ZERO, (a==0));
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 1);
  AssignFlag(CARRY, 0);
  ++pc;
  return 4;
}

uint8_t Cpu::AND_A_C() {
  a &= c;
  AssignFlag(ZERO, (a==0));
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 1);
  AssignFlag(CARRY, 0);
  ++pc;
  return 4;
}

uint8_t Cpu::AND_A_D() {
  a &= d;
  AssignFlag(ZERO, (a==0));
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 1);
  AssignFlag(CARRY, 0);
  ++pc;
  return 4;
}

uint8_t Cpu::AND_A_E() {
  a &= e;
  AssignFlag(ZERO, (a==0));
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 1);
  AssignFlag(CARRY, 0);
  ++pc;
  return 4;
}

uint8_t Cpu::AND_A_H() {
  a &= h;
  AssignFlag(ZERO, (a==0));
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 1);
  AssignFlag(CARRY, 0);
  ++pc;
  return 4;
}

uint8_t Cpu::AND_A_L() {
  a &= l;
  AssignFlag(ZERO, (a==0));
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 1);
  AssignFlag(CARRY, 0);
  ++pc;
  return 4;
}

/*  A6: AND (HL)
 *  Logical AND (HL) with A, store result in A. */
uint8_t Cpu::AND_A_atHL() {
  uint16_t address = (h << 8) | l;
  uint8_t val = bus->read(address);
  a &= val;
  AssignFlag(ZERO, (a==0));
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 1);
  AssignFlag(CARRY, 0);
  ++pc;
  return 8;
}

/*  AND_A_u8
 *  Logical AND n with A, store result in A. */
uint8_t Cpu::AND_A_u8() {
  uint8_t val = bus->read(++pc);
  a &= val;
  AssignFlag(ZERO, (a==0));
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 1);
  AssignFlag(CARRY, 0);
  ++pc;
  return 8;
}

/*  OR n
 *  Logical OR n with A, store result in A. */
uint8_t Cpu::OR_A_A() {
  a |= a;
  AssignFlag(ZERO, (a==0));
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 0);
  AssignFlag(CARRY, 0);
  ++pc;
  return 4;
}

uint8_t Cpu::OR_A_B() {
  a |= b;
  AssignFlag(ZERO, (a==0));
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 0);
  AssignFlag(CARRY, 0);
  ++pc;
  return 4;
}

uint8_t Cpu::OR_A_C() {
  a |= c;
  AssignFlag(ZERO, (a==0));
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 0);
  AssignFlag(CARRY, 0);
  ++pc;
  return 4;
}

uint8_t Cpu::OR_A_D() {
  a |= d;
  AssignFlag(ZERO, (a==0));
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 0);
  AssignFlag(CARRY, 0);
  ++pc;
  return 4;
}

uint8_t Cpu::OR_A_E() {
  a |= e;
  AssignFlag(ZERO, (a==0));
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 0);
  AssignFlag(CARRY, 0);
  ++pc;
  return 4;
}

uint8_t Cpu::OR_A_H() {
  a |= h;
  AssignFlag(ZERO, (a==0));
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 0);
  AssignFlag(CARRY, 0);
  ++pc;
  return 4;
}

uint8_t Cpu::OR_A_L() {
  a |= l;
  AssignFlag(ZERO, (a==0));
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 0);
  AssignFlag(CARRY, 0);
  ++pc;
  return 4;
}

/*  OR (HL)
 *  Logical OR (HL) with A, store result in A. */
uint8_t Cpu::OR_A_atHL() {
  uint16_t address = (h << 8) | l;
  uint8_t val = bus->read(address);
  a |= val;
  AssignFlag(ZERO, (a==0));
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 0);
  AssignFlag(CARRY, 0);
  ++pc;
  return 8;
}

/*  OR_A_u8
 *  Logical OR n with A, store result in A. */
uint8_t Cpu::OR_A_u8() {
  uint8_t val = bus->read(++pc);
  a |= val;
  AssignFlag(ZERO, (a==0));
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 0);
  AssignFlag(CARRY, 0);
  ++pc;
  return 8;
}

/*  XOR_A_u8
 *  Logical XOR n with A, store result in A. */
uint8_t Cpu::XOR_A_u8() {
  uint8_t val = bus->read(++pc);
  a ^= val;
  AssignFlag(ZERO, (a==0));
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 0);
  AssignFlag(CARRY, 0);
  ++pc;
  return 8;
}

/*  XOR n
 *  A ^= n */
uint8_t Cpu::XOR_A_A() {
  a ^= a;
  AssignFlag(ZERO, (a==0));
  AssignFlag(SUB, 0);
  AssignFlag(CARRY, 0);
  AssignFlag(HALF_CARRY, 0);
  ++pc;
  return 4;
}

uint8_t Cpu::XOR_A_B() {
  a ^= b;
  AssignFlag(ZERO, (b==0));
  AssignFlag(SUB, 0);
  AssignFlag(CARRY, 0);
  AssignFlag(HALF_CARRY, 0);
  ++pc;
  return 4;
}

uint8_t Cpu::XOR_A_C() {
  a ^= c;
  AssignFlag(ZERO, (c==0));
  AssignFlag(SUB, 0);
  AssignFlag(CARRY, 0);
  AssignFlag(HALF_CARRY, 0);
  ++pc;
  return 4;
}

uint8_t Cpu::XOR_A_D() {
  a ^= d;
  AssignFlag(ZERO, (d==0));
  AssignFlag(SUB, 0);
  AssignFlag(CARRY, 0);
  AssignFlag(HALF_CARRY, 0);
  ++pc;
  return 4;
}

uint8_t Cpu::XOR_A_E() {
  a ^= e;
  AssignFlag(ZERO, (e==0));
  AssignFlag(SUB, 0);
  AssignFlag(CARRY, 0);
  AssignFlag(HALF_CARRY, 0);
  ++pc;
  return 4;
}

uint8_t Cpu::XOR_A_H() {
  a ^= h;
  AssignFlag(ZERO, (h==0));
  AssignFlag(SUB, 0);
  AssignFlag(CARRY, 0);
  AssignFlag(HALF_CARRY, 0);
  ++pc;
  return 4;
}

uint8_t Cpu::XOR_A_L() {
  a ^= l;
  AssignFlag(ZERO, (l==0));
  AssignFlag(SUB, 0);
  AssignFlag(CARRY, 0);
  AssignFlag(HALF_CARRY, 0);
  ++pc;
  return 4;
}

/*  XOR (HL)
 *  Logical XOR (HL) with A, store result in A. */
uint8_t Cpu::XOR_A_atHL() {
  uint16_t address = (h << 8) | l;
  uint8_t val = bus->read(address);
  a ^= val;
  AssignFlag(ZERO, (a==0));
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 0);
  AssignFlag(CARRY, 0);
  ++pc;
  return 8;
}