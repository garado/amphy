
/* █░░ █▀█ ▄▀█ █▀▄ █▀    ▄▀█ █▄░█ █▀▄    █▀ ▀█▀ █▀█ █▀█ █▀▀ █▀ */ 
/* █▄▄ █▄█ █▀█ █▄▀ ▄█    █▀█ █░▀█ █▄▀    ▄█ ░█░ █▄█ █▀▄ ██▄ ▄█ */ 

#include "cpu.h"
#include <iomanip>

/* --- █▀█ ▄▄ █▄▄ █ ▀█▀ --- */ 
/* --- ███ ░░ █▄█ █ ░█░ --- */ 

/* LD n, (HL)
 * Put value in mem address in HL into register. */
void Cpu::LD_n_atHL(uint8_t * reg) {
  *reg = bus->read(hl());
  cycles_last_taken = 8;
}

void Cpu::LD_A_atHL() { LD_n_atHL(&a); }
void Cpu::LD_B_atHL() { LD_n_atHL(&b); }
void Cpu::LD_C_atHL() { LD_n_atHL(&c); }
void Cpu::LD_D_atHL() { LD_n_atHL(&d); }
void Cpu::LD_E_atHL() { LD_n_atHL(&e); }
void Cpu::LD_H_atHL() { LD_n_atHL(&h); }
void Cpu::LD_L_atHL() { LD_n_atHL(&l); }

/* LD (HL), n
 * Put contents of register n into mem location specified by HL. */
void Cpu::LD_atHL_n(uint8_t * reg) {
  bus->write(hl(), *reg);
  cycles_last_taken = 8;
}

void Cpu::LD_atHL_A() { LD_atHL_n(&a); }
void Cpu::LD_atHL_B() { LD_atHL_n(&b); }
void Cpu::LD_atHL_C() { LD_atHL_n(&c); }
void Cpu::LD_atHL_D() { LD_atHL_n(&d); }
void Cpu::LD_atHL_E() { LD_atHL_n(&e); }
void Cpu::LD_atHL_H() { LD_atHL_n(&h); }
void Cpu::LD_atHL_L() { LD_atHL_n(&l); }

/* 36: LD (HL), u8
 * Store contents of 8bit immediate n into mem location (HL) */
void Cpu::LD_atHL_u8() {
  uint8_t val = bus->read(pc++);
  bus->write(hl(), val);
  cycles_last_taken = 12;
}

/* LD n, u8
 * Load 8bit immediate value into register n. */
void Cpu::LD_n_u8(uint8_t * reg) {
  *reg = bus->read(pc++);
  cycles_last_taken = 8;
}

void Cpu::LD_A_u8() { LD_n_u8(&a); }
void Cpu::LD_B_u8() { LD_n_u8(&b); }
void Cpu::LD_C_u8() { LD_n_u8(&c); }
void Cpu::LD_D_u8() { LD_n_u8(&d); }
void Cpu::LD_E_u8() { LD_n_u8(&e); }
void Cpu::LD_H_u8() { LD_n_u8(&h); }
void Cpu::LD_L_u8() { LD_n_u8(&l); }

/* @Function  LD src, dst
 * @param     src   Pointer to source register
 *            dst   Pointer to dst register
 * @brief     Put value of reg n into reg m */
void Cpu::LD_n_m(uint8_t * dst, uint8_t * src) {
  *dst = *src;
  cycles_last_taken = 4;
}

void Cpu::LD_A_A() { LD_n_m(&a, &a); }
void Cpu::LD_A_B() { LD_n_m(&a, &b); }
void Cpu::LD_A_C() { LD_n_m(&a, &c); }
void Cpu::LD_A_D() { LD_n_m(&a, &d); }
void Cpu::LD_A_E() { LD_n_m(&a, &e); }
void Cpu::LD_A_H() { LD_n_m(&a, &h); }
void Cpu::LD_A_L() { LD_n_m(&a, &l); }

void Cpu::LD_B_A() { LD_n_m(&b, &a); }
void Cpu::LD_B_B() { LD_n_m(&b, &b); }
void Cpu::LD_B_C() { LD_n_m(&b, &c); }
void Cpu::LD_B_D() { LD_n_m(&b, &d); }
void Cpu::LD_B_E() { LD_n_m(&b, &e); }
void Cpu::LD_B_H() { LD_n_m(&b, &h); }
void Cpu::LD_B_L() { LD_n_m(&b, &l); }

void Cpu::LD_C_A() { LD_n_m(&c, &a); }
void Cpu::LD_C_B() { LD_n_m(&c, &b); }
void Cpu::LD_C_C() { LD_n_m(&c, &c); }
void Cpu::LD_C_D() { LD_n_m(&c, &d); }
void Cpu::LD_C_E() { LD_n_m(&c, &e); }
void Cpu::LD_C_H() { LD_n_m(&c, &h); }
void Cpu::LD_C_L() { LD_n_m(&c, &l); }

void Cpu::LD_D_A() { LD_n_m(&d, &a); }
void Cpu::LD_D_B() { LD_n_m(&d, &b); }
void Cpu::LD_D_C() { LD_n_m(&d, &c); }
void Cpu::LD_D_D() { LD_n_m(&d, &d); }
void Cpu::LD_D_E() { LD_n_m(&d, &e); }
void Cpu::LD_D_H() { LD_n_m(&d, &h); }
void Cpu::LD_D_L() { LD_n_m(&d, &l); }

void Cpu::LD_E_A() { LD_n_m(&e, &a); }
void Cpu::LD_E_B() { LD_n_m(&e, &b); }
void Cpu::LD_E_C() { LD_n_m(&e, &c); }
void Cpu::LD_E_D() { LD_n_m(&e, &d); }
void Cpu::LD_E_E() { LD_n_m(&e, &e); }
void Cpu::LD_E_H() { LD_n_m(&e, &h); }
void Cpu::LD_E_L() { LD_n_m(&e, &l); }

void Cpu::LD_H_A() { LD_n_m(&h, &a); }
void Cpu::LD_H_B() { LD_n_m(&h, &b); }
void Cpu::LD_H_C() { LD_n_m(&h, &c); }
void Cpu::LD_H_D() { LD_n_m(&h, &d); }
void Cpu::LD_H_E() { LD_n_m(&h, &e); }
void Cpu::LD_H_H() { LD_n_m(&h, &h); }
void Cpu::LD_H_L() { LD_n_m(&h, &l); }

void Cpu::LD_L_A() { LD_n_m(&l, &a); }
void Cpu::LD_L_B() { LD_n_m(&l, &b); }
void Cpu::LD_L_C() { LD_n_m(&l, &c); }
void Cpu::LD_L_D() { LD_n_m(&l, &d); }
void Cpu::LD_L_E() { LD_n_m(&l, &e); }
void Cpu::LD_L_H() { LD_n_m(&l, &h); }
void Cpu::LD_L_L() { LD_n_m(&l, &l); }

/* LD N, (M)
 * Put value in mem address into N. */
void Cpu::LD_n_atM(uint8_t * src, uint16_t address)
{
  *src = bus->read(address);
  cycles_last_taken = 8;
}

void Cpu::LD_A_atBC() { LD_n_atM( &a, bc() ); }
void Cpu::LD_A_atDE() { LD_n_atM( &a, de() ); }

/* E0: LD (a8), A
 * Store the contents of reg A in the address specified by $FF00 + 8-bit immediate. */
void Cpu::LDH_atu8_A() {
  uint16_t address = 0xFF00 | bus->read(pc++);
  bus->write(address, a);
  cycles_last_taken = 12;
}

/* F0: LD A, (a8)
 * Put value at address $FF00 + n into A. */
void Cpu::LDH_A_atu8() {
  uint16_t address = 0xFF00 | bus->read(pc++);
  a = bus->read(address);
  cycles_last_taken = 12;
}

/* F2: LD A, (C)
 * Put value at address $FF00 + RegC into A. */
void Cpu::LDH_A_atC() {
  uint16_t address = 0xFF00 | c;
  a = bus->read(address);
  cycles_last_taken = 8;
}

/* E2: LD (C), A
 * Put A in address $FF00 + RegC. */
void Cpu::LDH_atC_A() {
  uint16_t address = 0xFF00 | c;
  bus->write(address, a);
  cycles_last_taken = 8;
}

/* 3A: LD A, (HL-)
 * Put value at address HL into A. Decrement HL. */
void Cpu::LDD_A_atHL() {
  uint16_t address = hl();
  a = bus->read(address);

  // Decrement HL
  address--;
  hl(address);
  cycles_last_taken = 8;
}

/* 32: LD (HL-), A
 * Store contents of reg A into (HL). Decrement HL. */
void Cpu::LDD_atHL_A() {
  uint16_t address = hl();
  bus->write(address, a);

  // Decrement HL
  address--;
  hl(address);
  cycles_last_taken = 8;
}

/* 2A: LD A, (HL+)
 * Put val at address HL into A. Increment HL. */
void Cpu::LDI_A_atHL() {
  uint16_t address = hl();
  a = bus->read(address);

  // Increment HL
  address++;
  hl(address);
  cycles_last_taken = 8;
}

/* 22: LD (HL+), A
 * Put A into address HL. Increment HL. */
void Cpu::LDI_atHL_A() {
  uint16_t address = hl();
  bus->write(address, a);

  // Increment HL
  address++;
  hl(address);
  cycles_last_taken = 8;
}

/* F8: LD HL, SP+i8
 * Add 8-bit signed operand i8 (-128 to 127) to SP.
 * Store result in HL. */
void Cpu::LD_HL_SP_i8() {
  int8_t i8 = bus->read(pc++);
  uint16_t newHL = sp + i8;
  hl(newHL);
  SetAddFlags(sp, (int16_t) i8);
  AssignFlag(ZERO, 0);
  cycles_last_taken = 12;
}

/* --- ▄█ █▄▄ ▄▄ █▄▄ █ ▀█▀ --- */ 
/* --- ░█ █▄█ ░░ █▄█ █ ░█░ --- */ 

/* FA: LD A, (nn)
 * Load value in mem address specified by 16-bit immediate nn into A. */
void Cpu::LD_A_atu16() {
  uint8_t lsb = bus->read(pc++);
  uint8_t msb = bus->read(pc++);
  uint16_t address = (msb << 8) | lsb;
  a = bus->read(address);
  cycles_last_taken = 16;
}

/* EA: LD (a16), A
 * Store contents of register A in address specified by a16. */
void Cpu::LD_atu16_A() {
  uint8_t lsb = bus->read(pc++);
  uint8_t msb = bus->read(pc++);
  uint16_t address = (msb << 8) | lsb;
  bus->write(address, a);
  cycles_last_taken = 16;
}

/* 01: LD BC, d16
 * Put immed val d16 into register pair BC */
void Cpu::LD_BC_u16() {
  c = bus->read(pc++); // LSB of d16
  b = bus->read(pc++); // MSB of d16
  cycles_last_taken = 12;
}

void Cpu::LD_DE_u16() {
  e = bus->read(pc++); // LSB of d16
  d = bus->read(pc++); // MSB of d16
  cycles_last_taken = 12;
}

/* 31: LD SP, d16
 * Put immed val d16 into SP */
void Cpu::LD_SP_u16() {
  uint8_t lsb = bus->read(pc++);
  uint8_t msb = bus->read(pc++);
  sp = (msb << 8) | lsb;
  cycles_last_taken = 12;
}

/* 31: LD HL, d16
 * Put immed val d16 into HL */
void Cpu::LD_HL_u16() {
  l = bus->read(pc++); // LSB of nn
  h = bus->read(pc++); // MSB of nn
  cycles_last_taken = 12;
}

/* 02: LD (BC), A
 * Store contents of A into mem location specified by BC. */
void Cpu::LD_atBC_A() {
  bus->write(bc(), a);
  cycles_last_taken = 8;
}

/* 12: LD (BC), A
 * Store contents of A into mem location specified by DE. */
void Cpu::LD_atDE_A() {
  bus->write(de(), a);
  cycles_last_taken = 8;
}

/* 08: LD (U16), SP
 * Store the lower byte of stack pointer SP at the address specified by
 * 16-bit immediate operand a16, and store the upper byte of SP at
 * address a16 + 1. */
void Cpu::LD_atu16_SP() {
  uint16_t address = bus->read(pc++);
  address |= bus->read(pc++) << 8;
  bus->write(address, sp & 0xFF);
  bus->write(++address, sp >> 8);
  cycles_last_taken = 20;
}

/* F9: LD SP, HL
 * Put HL into SP. */
void Cpu::LD_SP_HL() {
  sp = hl();
  cycles_last_taken = 8;
}

void Cpu::Push_NM(uint16_t regpair) {
  // check for underflow later because i can't find what the stack
  // range is
  uint8_t lower = regpair >> 8;
  uint8_t upper = regpair & 0xFF;
  bus->write(--sp, lower);
  bus->write(--sp, upper);
  cycles_last_taken = 16;
}

void Cpu::PUSH_AF() { Push_NM( af() ); }
void Cpu::PUSH_BC() { Push_NM( bc() ); }
void Cpu::PUSH_DE() { Push_NM( de() ); }
void Cpu::PUSH_HL() { Push_NM( hl() ); }

/* Pop_NM
 * Pop SP to register pair. Increment SP twice. */
void Cpu::Pop_NM(uint8_t * upper, uint8_t * lower) {
  *lower = bus->read(sp++);
  *upper = bus->read(sp++);
  cycles_last_taken = 16;
}

void Cpu::POP_BC() { Pop_NM( &b, &c ); }
void Cpu::POP_DE() { Pop_NM( &d, &e ); }
void Cpu::POP_HL() { Pop_NM( &h, &l ); }

// special handling
void Cpu::POP_AF() {
  f = (bus->read(sp++)) & 0xF0;
  a = bus->read(sp++);
  cycles_last_taken = 16;
}
