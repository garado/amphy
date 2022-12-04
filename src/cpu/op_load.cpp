
/* █░░ █▀█ ▄▀█ █▀▄ █▀    ▄▀█ █▄░█ █▀▄    █▀ ▀█▀ █▀█ █▀█ █▀▀ █▀ */ 
/* █▄▄ █▄█ █▀█ █▄▀ ▄█    █▀█ █░▀█ █▄▀    ▄█ ░█░ █▄█ █▀▄ ██▄ ▄█ */ 

#include "cpu.h"

/* --- █▀█ ▄▄ █▄▄ █ ▀█▀ --- */ 
/* --- ███ ░░ █▄█ █ ░█░ --- */ 

/* LD n, (HL)
 * Put value in mem address in HL into register. */
uint8_t Cpu::LD_n_atHL(uint8_t * reg) {
  uint16_t address = hl();
  *reg = bus->read(address);
  ++pc;
  return 8;
}

uint8_t Cpu::LD_A_atHL() { return LD_n_atHL(&a); }
uint8_t Cpu::LD_B_atHL() { return LD_n_atHL(&b); }
uint8_t Cpu::LD_C_atHL() { return LD_n_atHL(&c); }
uint8_t Cpu::LD_D_atHL() { return LD_n_atHL(&d); }
uint8_t Cpu::LD_E_atHL() { return LD_n_atHL(&e); }
uint8_t Cpu::LD_H_atHL() { return LD_n_atHL(&h); }
uint8_t Cpu::LD_L_atHL() { return LD_n_atHL(&l); }

/* LD (HL), n
 * Put contents of register n into mem location specified by HL. */
uint8_t Cpu::LD_atHL_n(uint8_t * reg) {
  uint16_t address = hl();
  bus->write(address, *reg);
  ++pc;
  return 8;
}

uint8_t Cpu::LD_atHL_A() { return LD_atHL_n(&a); }
uint8_t Cpu::LD_atHL_B() { return LD_atHL_n(&b); }
uint8_t Cpu::LD_atHL_C() { return LD_atHL_n(&c); }
uint8_t Cpu::LD_atHL_D() { return LD_atHL_n(&d); }
uint8_t Cpu::LD_atHL_E() { return LD_atHL_n(&e); }
uint8_t Cpu::LD_atHL_H() { return LD_atHL_n(&h); }
uint8_t Cpu::LD_atHL_L() { return LD_atHL_n(&l); }

/* 36: LD (HL), u8
 * Store contents of 8bit immediate n into mem location (HL) */
uint8_t Cpu::LD_atHL_u8() {
  uint16_t address = hl();
  uint8_t val = bus->read(++pc);
  bus->write(address, val);
  ++pc;
  return 12;
}

/* LD n, u8
 * Load 8bit immediate value into register n. */
uint8_t Cpu::LD_n_u8(uint8_t * reg) {
  uint8_t u8 = bus->read(++pc);
  *reg = u8;
  ++pc;
  return 8;
}

uint8_t Cpu::LD_A_u8() { return LD_n_u8(&a); }
uint8_t Cpu::LD_B_u8() { return LD_n_u8(&b); }
uint8_t Cpu::LD_C_u8() { return LD_n_u8(&c); }
uint8_t Cpu::LD_D_u8() { return LD_n_u8(&d); }
uint8_t Cpu::LD_E_u8() { return LD_n_u8(&e); }
uint8_t Cpu::LD_H_u8() { return LD_n_u8(&h); }
uint8_t Cpu::LD_L_u8() { return LD_n_u8(&l); }

/* @Function  LD src, dst
 * @param     src   Pointer to source register
 *            dst   Pointer to dst register
 * @brief     Put value of reg n into reg m */
uint8_t Cpu::LD_n_m(uint8_t * src, uint8_t * dst) {
  *dst = *src;
  ++pc;
  return 4;
}

uint8_t Cpu::LD_A_A() { return LD_n_m(&a, &a); }
uint8_t Cpu::LD_A_B() { return LD_n_m(&b, &a); }
uint8_t Cpu::LD_A_C() { return LD_n_m(&c, &a); }
uint8_t Cpu::LD_A_D() { return LD_n_m(&d, &a); }
uint8_t Cpu::LD_A_E() { return LD_n_m(&e, &a); }
uint8_t Cpu::LD_A_H() { return LD_n_m(&h, &a); }
uint8_t Cpu::LD_A_L() { return LD_n_m(&l, &a); }

uint8_t Cpu::LD_B_A() { return LD_n_m(&a, &b); }
uint8_t Cpu::LD_B_B() { return LD_n_m(&b, &b); }
uint8_t Cpu::LD_B_C() { return LD_n_m(&c, &b); }
uint8_t Cpu::LD_B_D() { return LD_n_m(&d, &b); }
uint8_t Cpu::LD_B_E() { return LD_n_m(&e, &b); }
uint8_t Cpu::LD_B_H() { return LD_n_m(&h, &b); }
uint8_t Cpu::LD_B_L() { return LD_n_m(&l, &b); }

uint8_t Cpu::LD_C_A() { return LD_n_m(&a, &c); }
uint8_t Cpu::LD_C_B() { return LD_n_m(&b, &c); }
uint8_t Cpu::LD_C_C() { return LD_n_m(&c, &c); }
uint8_t Cpu::LD_C_D() { return LD_n_m(&d, &c); }
uint8_t Cpu::LD_C_E() { return LD_n_m(&e, &c); }
uint8_t Cpu::LD_C_H() { return LD_n_m(&h, &c); }
uint8_t Cpu::LD_C_L() { return LD_n_m(&l, &c); }

uint8_t Cpu::LD_D_A() { return LD_n_m(&a, &d); }
uint8_t Cpu::LD_D_B() { return LD_n_m(&b, &d); }
uint8_t Cpu::LD_D_C() { return LD_n_m(&c, &d); }
uint8_t Cpu::LD_D_D() { return LD_n_m(&d, &d); }
uint8_t Cpu::LD_D_E() { return LD_n_m(&e, &d); }
uint8_t Cpu::LD_D_H() { return LD_n_m(&h, &d); }
uint8_t Cpu::LD_D_L() { return LD_n_m(&l, &d); }

uint8_t Cpu::LD_E_A() { return LD_n_m(&a, &e); }
uint8_t Cpu::LD_E_B() { return LD_n_m(&b, &e); }
uint8_t Cpu::LD_E_C() { return LD_n_m(&c, &e); }
uint8_t Cpu::LD_E_D() { return LD_n_m(&d, &e); }
uint8_t Cpu::LD_E_E() { return LD_n_m(&e, &e); }
uint8_t Cpu::LD_E_H() { return LD_n_m(&h, &e); }
uint8_t Cpu::LD_E_L() { return LD_n_m(&l, &e); }

uint8_t Cpu::LD_H_A() { return LD_n_m(&a, &h); }
uint8_t Cpu::LD_H_B() { return LD_n_m(&b, &h); }
uint8_t Cpu::LD_H_C() { return LD_n_m(&c, &h); }
uint8_t Cpu::LD_H_D() { return LD_n_m(&d, &h); }
uint8_t Cpu::LD_H_E() { return LD_n_m(&e, &h); }
uint8_t Cpu::LD_H_H() { return LD_n_m(&h, &h); }
uint8_t Cpu::LD_H_L() { return LD_n_m(&l, &h); }

uint8_t Cpu::LD_L_A() { return LD_n_m(&a, &l); }
uint8_t Cpu::LD_L_B() { return LD_n_m(&b, &l); }
uint8_t Cpu::LD_L_C() { return LD_n_m(&c, &l); }
uint8_t Cpu::LD_L_D() { return LD_n_m(&d, &l); }
uint8_t Cpu::LD_L_E() { return LD_n_m(&e, &l); }
uint8_t Cpu::LD_L_H() { return LD_n_m(&h, &l); }
uint8_t Cpu::LD_L_L() { return LD_n_m(&l, &l); }

/* LD N, (M)
 * Put value in mem address into N. */
uint8_t Cpu::LD_n_atM(uint8_t * src, uint16_t address)
{
  uint8_t val = bus->read(address);
  *src = val;
  ++pc;
  return 8;
}

uint8_t Cpu::LD_A_atBC() { return LD_n_atM( &a, bc() ); }
uint8_t Cpu::LD_A_atDE() { return LD_n_atM( &a, de() ); }

/* E0: LD (a8), A
 * Store the contents of reg A in the address specified by $FF00 + 8-bit immediate. */
uint8_t Cpu::LDH_atu8_A() {
  uint8_t a8 = bus->read(++pc);
  uint16_t address = 0xFF00 + a8;
  bus->write(address, a);
  ++pc;
  return 12;
}

/* F0: LD A, (a8)
 * Put value at address $FF00 + n into A. */
uint8_t Cpu::LDH_A_atu8() {
  uint8_t a8 = bus->read(++pc);
  uint16_t address = 0xFF00 + a8;
  a = bus->read(address);
  ++pc;
  return 12;
}

/* F2: LD A, (C)
 * Put value at address $FF00 + RegC into A. */
uint8_t Cpu::LDH_A_atC() {
  uint16_t address = 0xFF00 | c;
  a = bus->read(address);
  ++pc;
  return 8;
}

/* E2: LD (C), A
 * Put A in address $FF00 + RegC. */
uint8_t Cpu::LDH_atC_A() {
  uint16_t address = 0xFF00 | c;
  bus->write(address, a);
  ++pc;
  return 8;
}

/* 3A: LD A, (HL-)
 * Put value at address HL into A. Decrement HL. */
uint8_t Cpu::LDD_A_atHL() {
  uint16_t address = hl();
  a = bus->read(address);

  // Decrement HL
  address--;
  h = address >> 8;
  l = (address & 0xFF);
  ++pc;
  return 8;
}

/* 32: LD (HL-), A
 * Store contents of reg A into (HL). Decrement HL. */
uint8_t Cpu::LDD_atHL_A() {
  uint16_t address = hl();
  bus->write(address, a);

  // Decrement HL
  address--;
  h = address >> 8;
  l = (address & 0xFF);
  ++pc;
  return 8;
}

/* 2A: LD A, (HL+)
 * Put val at address HL into A. Increment HL. */
uint8_t Cpu::LDI_A_atHL() {
  uint16_t address = hl();
  a = bus->read(address);

  // Increment HL
  address++;
  h = address >> 8;
  l = (address & 0xFF);
  ++pc;
  return 8;
}

/* 22: LD (HL+), A
 * Put A into address HL. Increment HL. */
uint8_t Cpu::LDI_atHL_A() {
  uint16_t address = hl();
  bus->write(address, a);

  // Increment HL
  address++;
  h = address >> 8;
  l = (address & 0xFF);
  ++pc;
  return 8;
}

/* F8: LD HL, SP+i8
 * Add 8-bit signed operand i8 (-128 to 127) to SP.
 * Store result in HL. */
uint8_t Cpu::LD_HL_SP_i8() {
  int8_t i8 = bus->read(++pc);
  uint16_t newHL = sp + i8;
  h = (newHL & 0xFF00) >> 8;
  l = newHL & 0xFF;
  SetAddFlags(sp, (int16_t) i8);
  AssignFlag(ZERO, 0);
  ++pc;
  return 12;
}

/* --- ▄█ █▄▄ ▄▄ █▄▄ █ ▀█▀ --- */ 
/* --- ░█ █▄█ ░░ █▄█ █ ░█░ --- */ 

/* FA: LD A, (nn)
 * Load value in mem address specified by 16-bit immediate nn into A. */
uint8_t Cpu::LD_A_atu16() {
  uint8_t lsb = bus->read(++pc);
  uint8_t msb = bus->read(++pc);
  uint16_t address = (msb << 8) | lsb;
  a = bus->read(address);
  ++pc;
  return 16;
}

/* EA: LD (a16), A
 * Store contents of register A in address specified by a16. */
uint8_t Cpu::LD_atu16_A() {
  uint8_t lsb = bus->read(++pc);
  uint8_t msb = bus->read(++pc);
  uint16_t address = (msb << 8) | lsb;
  bus->write(address, a);
  ++pc;
  return 16;
}

/* 01: LD BC, d16
 * Put immed val d16 into register pair BC */
uint8_t Cpu::LD_BC_u16() {
  c = bus->read(++pc); // LSB of d16
  b = bus->read(++pc); // MSB of d16
  ++pc;
  return 12;
}

uint8_t Cpu::LD_DE_u16() {
  e = bus->read(++pc); // LSB of d16
  d = bus->read(++pc); // MSB of d16
  ++pc;
  return 12;
}

/* 31: LD SP, d16
 * Put immed val d16 into SP */
uint8_t Cpu::LD_SP_u16() {
  uint8_t lsb = bus->read(++pc);
  uint8_t msb = bus->read(++pc);
  sp = (msb << 8) | lsb;
  ++pc;
  return 12;
}

/* 31: LD HL, d16
 * Put immed val d16 into HL */
uint8_t Cpu::LD_HL_u16() {
  l = bus->read(++pc); // LSB of nn
  h = bus->read(++pc); // MSB of nn
  ++pc;
  return 12;
}

/* 02: LD (BC), A
 * Store contents of A into mem location specified by BC. */
uint8_t Cpu::LD_atBC_A() {
  uint16_t address = bc();
  bus->write(address, a);
  ++pc;
  return 8;
}

/* 12: LD (BC), A
 * Store contents of A into mem location specified by DE. */
uint8_t Cpu::LD_atDE_A() {
  uint16_t address = de();
  bus->write(address, a);
  ++pc;
  return 8;
}

/* 08: LD (U16), SP
 * Store the lower byte of stack pointer SP at the address specified by
 * 16-bit immediate operand a16, and store the upper byte of SP at
 * address a16 + 1. */
uint8_t Cpu::LD_atu16_SP() {
  uint8_t msb = sp >> 8;
  uint8_t lsb = sp & 0xFF;
  bus->write(++pc, lsb);
  bus->write(++pc, msb);
  ++pc;
  return 20;
}

/* F9: LD SP, HL
 * Put HL into SP. */
uint8_t Cpu::LD_SP_HL() {
  sp = hl();
  ++pc;
  return 8;
}

uint8_t Cpu::Push_NM(uint16_t regpair) {
  // check for underflow later because i can't find what the stack
  // range is
  uint8_t lower = regpair >> 8;
  uint8_t upper = regpair & 0xFF;
  bus->write(--sp, lower);
  bus->write(--sp, upper);
  ++pc;
  return 16;
}

uint8_t Cpu::PUSH_AF() { return Push_NM( af() ); }
uint8_t Cpu::PUSH_BC() { return Push_NM( bc() ); }
uint8_t Cpu::PUSH_DE() { return Push_NM( de() ); }
uint8_t Cpu::PUSH_HL() { return Push_NM( hl() ); }

/* Pop_NM
 * Pop SP to register pair. Increment SP twice. */
uint8_t Cpu::Pop_NM(uint8_t * upper, uint8_t * lower) {
  *lower = bus->read(sp++);
  *upper = bus->read(sp++);
  ++pc;
  return 16;
}

uint8_t Cpu::POP_AF() { return Pop_NM( &a, &f ); }
uint8_t Cpu::POP_BC() { return Pop_NM( &b, &c ); }
uint8_t Cpu::POP_DE() { return Pop_NM( &d, &e ); }
uint8_t Cpu::POP_HL() { return Pop_NM( &h, &l ); }
