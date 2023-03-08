
/* █▄▄ █ ▀█▀ █░█░█ █ █▀ █▀▀    █▀█ █▀█ █▀▀ █▀█ █▀▄ █▀▀ █▀ */ 
/* █▄█ █ ░█░ ▀▄▀▄▀ █ ▄█ ██▄    █▄█ █▀▀ █▄▄ █▄█ █▄▀ ██▄ ▄█ */ 

#include "cpu.h"
#include "../defines.h"

/* AND n
 * Logical AND reg n with A; store result in A. */
void Cpu::AND_A_n(uint8_t * reg)
{
  a &= *reg;
  AssignFlag(ZERO, (a==0));
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 1);
  AssignFlag(CARRY, 0);
  cycles_last_taken = 4;
}

void Cpu::AND_A_A() { AND_A_n(&a); }
void Cpu::AND_A_B() { AND_A_n(&b); }
void Cpu::AND_A_C() { AND_A_n(&c); }
void Cpu::AND_A_D() { AND_A_n(&d); }
void Cpu::AND_A_E() { AND_A_n(&e); }
void Cpu::AND_A_H() { AND_A_n(&h); }
void Cpu::AND_A_L() { AND_A_n(&l); }

/* A6: AND (HL)
 * Logical AND (HL) with A, store result in A. */
void Cpu::AND_A_atHL() {
  uint8_t val = bus->read( hl() );
  a &= val;
  AssignFlag(ZERO, (a==0));
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 1);
  AssignFlag(CARRY, 0);
  cycles_last_taken = 8;
}

/* AND_A_u8
 * Logical AND n with A, store result in A. */
void Cpu::AND_A_u8() {
  uint8_t val = bus->read(++pc);
  a &= val;
  AssignFlag(ZERO, (a==0));
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 1);
  AssignFlag(CARRY, 0);
  cycles_last_taken = 8;
}

/* F6: OR n
 * Logical OR reg n with A; store result in A. */
void Cpu::OR_A_n(uint8_t * reg)
{
  a |= *reg;
  AssignFlag(ZERO, (a==0));
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 0);
  AssignFlag(CARRY, 0);
  cycles_last_taken = 4;
}

void Cpu::OR_A_A() { OR_A_n(&a); }
void Cpu::OR_A_B() { OR_A_n(&b); }
void Cpu::OR_A_C() { OR_A_n(&c); }
void Cpu::OR_A_D() { OR_A_n(&d); }
void Cpu::OR_A_E() { OR_A_n(&e); }
void Cpu::OR_A_H() { OR_A_n(&h); }
void Cpu::OR_A_L() { OR_A_n(&l); }

/*  OR (HL)
 *  Logical OR (HL) with A, store result in A. */
void Cpu::OR_A_atHL() {
  uint8_t val = bus->read( hl() );
  a |= val;
  AssignFlag(ZERO, (a==0));
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 0);
  AssignFlag(CARRY, 0);
  cycles_last_taken = 8;
}

/*  OR_A_u8
 *  Logical OR n with A, store result in A. */
void Cpu::OR_A_u8() {
  uint8_t val = bus->read(pc++);
  a |= val;
  AssignFlag(ZERO, (a==0));
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 0);
  AssignFlag(CARRY, 0);
  cycles_last_taken = 8;
}

/*  XOR_A_u8
 *  Logical XOR n with A, store result in A. */
void Cpu::XOR_A_u8() {
  uint8_t val = bus->read(pc++);
  a ^= val;
  AssignFlag(ZERO, (a==0));
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 0);
  AssignFlag(CARRY, 0);
  cycles_last_taken = 8;
}


/* XOR n
 * Logical XOR reg n with A; store result in A. */
void Cpu::XOR_A_n(uint8_t * reg)
{
  a ^= *reg;
  AssignFlag(ZERO, (a==0));
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 0);
  AssignFlag(CARRY, 0);
  cycles_last_taken = 4;
}

void Cpu::XOR_A_A() { XOR_A_n(&a); }
void Cpu::XOR_A_B() { XOR_A_n(&b); }
void Cpu::XOR_A_C() { XOR_A_n(&c); }
void Cpu::XOR_A_D() { XOR_A_n(&d); }
void Cpu::XOR_A_E() { XOR_A_n(&e); }
void Cpu::XOR_A_H() { XOR_A_n(&h); }
void Cpu::XOR_A_L() { XOR_A_n(&l); }

/*  XOR (HL)
 *  Logical XOR (HL) with A, store result in A. */
void Cpu::XOR_A_atHL() {
  uint8_t val = bus->read( hl() );
  a ^= val;
  AssignFlag(ZERO, (a==0));
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 0);
  AssignFlag(CARRY, 0);
  cycles_last_taken = 8;
}
