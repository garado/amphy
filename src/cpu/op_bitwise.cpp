
/* █▄▄ █ ▀█▀ █░█░█ █ █▀ █▀▀    █▀█ █▀█ █▀▀ █▀█ █▀▄ █▀▀ █▀ */ 
/* █▄█ █ ░█░ ▀▄▀▄▀ █ ▄█ ██▄    █▄█ █▀▀ █▄▄ █▄█ █▄▀ ██▄ ▄█ */ 

#include "cpu.h"

/* AND n
 * Logical AND reg n with A; store result in A. */
uint8_t Cpu::AND_A_n(uint8_t * reg)
{
  a &= *reg;
  AssignFlag(ZERO, (a==0));
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 1);
  AssignFlag(CARRY, 0);
  return 4;
}

uint8_t Cpu::AND_A_A() { return AND_A_n(&a); }
uint8_t Cpu::AND_A_B() { return AND_A_n(&b); }
uint8_t Cpu::AND_A_C() { return AND_A_n(&c); }
uint8_t Cpu::AND_A_D() { return AND_A_n(&d); }
uint8_t Cpu::AND_A_E() { return AND_A_n(&e); }
uint8_t Cpu::AND_A_H() { return AND_A_n(&h); }
uint8_t Cpu::AND_A_L() { return AND_A_n(&l); }

/* A6: AND (HL)
 * Logical AND (HL) with A, store result in A. */
uint8_t Cpu::AND_A_atHL() {
  uint8_t val = bus->read( hl() );
  a &= val;
  AssignFlag(ZERO, (a==0));
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 1);
  AssignFlag(CARRY, 0);
  return 8;
}

/* AND_A_u8
 * Logical AND n with A, store result in A. */
uint8_t Cpu::AND_A_u8() {
  uint8_t val = bus->read(++pc);
  a &= val;
  AssignFlag(ZERO, (a==0));
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 1);
  AssignFlag(CARRY, 0);
  return 8;
}

/* OR n
 * Logical OR reg n with A; store result in A. */
uint8_t Cpu::OR_A_n(uint8_t * reg)
{
  a |= *reg;
  AssignFlag(ZERO, (a==0));
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 0);
  AssignFlag(CARRY, 0);
  return 4;
}

uint8_t Cpu::OR_A_A() { return OR_A_n(&a); }
uint8_t Cpu::OR_A_B() { return OR_A_n(&b); }
uint8_t Cpu::OR_A_C() { return OR_A_n(&c); }
uint8_t Cpu::OR_A_D() { return OR_A_n(&d); }
uint8_t Cpu::OR_A_E() { return OR_A_n(&e); }
uint8_t Cpu::OR_A_H() { return OR_A_n(&h); }
uint8_t Cpu::OR_A_L() { return OR_A_n(&l); }

/*  OR (HL)
 *  Logical OR (HL) with A, store result in A. */
uint8_t Cpu::OR_A_atHL() {
  uint8_t val = bus->read( hl() );
  a |= val;
  AssignFlag(ZERO, (a==0));
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 0);
  AssignFlag(CARRY, 0);
  return 8;
}

/*  OR_A_u8
 *  Logical OR n with A, store result in A. */
uint8_t Cpu::OR_A_u8() {
  uint8_t val = bus->read(pc++);
  a |= val;
  AssignFlag(ZERO, (a==0));
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 0);
  AssignFlag(CARRY, 0);
  return 8;
}

/*  XOR_A_u8
 *  Logical XOR n with A, store result in A. */
uint8_t Cpu::XOR_A_u8() {
  uint8_t val = bus->read(pc++);
  a ^= val;
  AssignFlag(ZERO, (a==0));
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 0);
  AssignFlag(CARRY, 0);
  return 8;
}


/* XOR n
 * Logical OR reg n with A; store result in A. */
uint8_t Cpu::XOR_A_n(uint8_t * reg)
{
  a ^= *reg;
  AssignFlag(ZERO, (a==0));
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 1);
  AssignFlag(CARRY, 0);
  return 4;
}

uint8_t Cpu::XOR_A_A() { return XOR_A_n(&a); }
uint8_t Cpu::XOR_A_B() { return XOR_A_n(&b); }
uint8_t Cpu::XOR_A_C() { return XOR_A_n(&c); }
uint8_t Cpu::XOR_A_D() { return XOR_A_n(&d); }
uint8_t Cpu::XOR_A_E() { return XOR_A_n(&e); }
uint8_t Cpu::XOR_A_H() { return XOR_A_n(&h); }
uint8_t Cpu::XOR_A_L() { return XOR_A_n(&l); }

/*  XOR (HL)
 *  Logical XOR (HL) with A, store result in A. */
uint8_t Cpu::XOR_A_atHL() {
  uint8_t val = bus->read( hl() );
  a ^= val;
  AssignFlag(ZERO, (a==0));
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 0);
  AssignFlag(CARRY, 0);
  return 8;
}
