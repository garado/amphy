
/* ▄▀█ █░░ █░█ */ 
/* █▀█ █▄▄ █▄█ */ 

#include "cpu.h"

/* ========== 8-BIT ALU ========== */

/* 34: INC HL
 * Increment contents of (HL). */
uint8_t Cpu::INC_atHL() {
  uint8_t val = bus->read( hl() );
  SetAddFlags(val, 1);
  ++val;
  bus->write(hl(), val);
  return 12;
}

/* 35: DEC HL
 * Decrement contents of (HL). */
uint8_t Cpu::DEC_atHL() {
  uint8_t val = bus->read(hl());
  SetSubFlags(val, 1);
  --val;
  bus->write(hl(), val);
  return 12;
}

/* 86: ADD A, (HL)
 * A = A + (HL) */
uint8_t Cpu::ADD_A_atHL() {
  uint16_t address = hl();
  uint8_t val = bus->read(address);
  SetAddFlags(a, val);
  a += val;
  return 8;
}

/*  C6: ADD A, u8
 *  A = A + u8 */
uint8_t Cpu::ADD_A_u8() {
  int8_t u8 = bus->read(pc++);
  SetAddFlags(a, u8);
  a += u8;
  return 8;
}

/*  96: SUB (HL)
 *  Subtract (HL) from A. */
uint8_t Cpu::SUB_A_atHL() {
  uint8_t val = bus->read( hl() );
  SetSubFlags(a, val);
  a -= val;
  return 8;
}

/*  D6: SUB n
 *  Subtract 8bit immed u8 from A */
uint8_t Cpu::SUB_A_u8() {
  uint8_t u8 = bus->read(pc++);
  SetSubFlags(a, u8);
  a -= u8;
  return 8;
}

// -------------------------------

/* INC_n
 * Increment the contents of the specified register.
 * This DOES NOT set the carry flag (so we can't use SetAddFlags) */
uint8_t Cpu::INC_n(uint8_t * reg)
{
  SetHalfCarryAdd(*reg, 1);
  AssignFlag(SUB, 0);
  *reg = *reg + 1;
  AssignFlag(ZERO, (*reg == 0));
  return 4;
}

uint8_t Cpu::INC_A() { return INC_n(&a); }
uint8_t Cpu::INC_B() { return INC_n(&b); }
uint8_t Cpu::INC_C() { return INC_n(&c); }
uint8_t Cpu::INC_D() { return INC_n(&d); }
uint8_t Cpu::INC_E() { return INC_n(&e); }
uint8_t Cpu::INC_H() { return INC_n(&h); }
uint8_t Cpu::INC_L() { return INC_n(&l); }

/* DEC_n
 * Decrement the contents of the specified register.
 * This DOES NOT set the carry flag (so we can't use SetSubFlags) */
uint8_t Cpu::DEC_n(uint8_t * reg)
{
  AssignFlag(SUB, 1);
  SetHalfCarrySub(*reg, 1);
  *reg = *reg - 1;
  AssignFlag(ZERO, (*reg == 0));
  return 4;
}

uint8_t Cpu::DEC_A() { return DEC_n(&a); }
uint8_t Cpu::DEC_B() { return DEC_n(&b); }
uint8_t Cpu::DEC_C() { return DEC_n(&c); }
uint8_t Cpu::DEC_D() { return DEC_n(&d); }
uint8_t Cpu::DEC_E() { return DEC_n(&e); }
uint8_t Cpu::DEC_H() { return DEC_n(&h); }
uint8_t Cpu::DEC_L() { return DEC_n(&l); }

/* ADD A, n
 * Add the contents of reg n to reg a.
 * Store the result in reg a. */
uint8_t Cpu::ADD_A_n(uint8_t * reg)
{
  SetAddFlags(a, *reg);
  a += *reg;
  return 4;
}

uint8_t Cpu::ADD_A_A() { return ADD_A_n(&a); }
uint8_t Cpu::ADD_A_B() { return ADD_A_n(&b); }
uint8_t Cpu::ADD_A_C() { return ADD_A_n(&c); }
uint8_t Cpu::ADD_A_D() { return ADD_A_n(&d); }
uint8_t Cpu::ADD_A_E() { return ADD_A_n(&e); }
uint8_t Cpu::ADD_A_H() { return ADD_A_n(&h); }
uint8_t Cpu::ADD_A_L() { return ADD_A_n(&l); }

/* SUB A, n
 * Subtract the contents of reg n from reg a.
 * Store the result in reg a. */
uint8_t Cpu::SUB_A_n(uint8_t * reg)
{
  SetSubFlags(a, *reg);
  a -= *reg;
  return 4;
}

uint8_t Cpu::SUB_A_A() { return SUB_A_n(&a); }
uint8_t Cpu::SUB_A_B() { return SUB_A_n(&b); }
uint8_t Cpu::SUB_A_C() { return SUB_A_n(&c); }
uint8_t Cpu::SUB_A_D() { return SUB_A_n(&d); }
uint8_t Cpu::SUB_A_E() { return SUB_A_n(&e); }
uint8_t Cpu::SUB_A_H() { return SUB_A_n(&h); }
uint8_t Cpu::SUB_A_L() { return SUB_A_n(&l); }

/* SBC A, n
 * Subtract value of reg n + carry from reg a.
 * A = A - (R1 + CY) */
uint8_t Cpu::SBC_A_n(uint8_t * reg)
{
  uint8_t val = *reg + GetFlag(CARRY);
  SetSubFlags(a, val);
  a -= val;
  return 4;
}

uint8_t Cpu::SBC_A_A() { return SBC_A_n(&a); }
uint8_t Cpu::SBC_A_B() { return SBC_A_n(&b); }
uint8_t Cpu::SBC_A_C() { return SBC_A_n(&c); }
uint8_t Cpu::SBC_A_D() { return SBC_A_n(&d); }
uint8_t Cpu::SBC_A_E() { return SBC_A_n(&e); }
uint8_t Cpu::SBC_A_H() { return SBC_A_n(&h); }
uint8_t Cpu::SBC_A_L() { return SBC_A_n(&l); }

/*  SBC A, (HL)
 *  Subtract value of (HL) + carry from A. 
 *  A = A - ((HL) + CY) */
uint8_t Cpu::SBC_A_atHL() {
  uint8_t val = bus->read(hl()) + GetFlag(CARRY);
  SetSubFlags(a, val);
  a -= val;
  return 8;
}

/*  DE: SBC A, u8
 *  Subtract u8 + carry from A. 
 *  A = A - (u8 + CY) */
uint8_t Cpu::SBC_A_u8() {
  uint8_t u8 = bus->read(pc++);
  uint8_t val = u8 + GetFlag(CARRY);
  SetSubFlags(a, val);
  a -= val;
  return 4;
}

/* ADC A, n
 * Add value of reg n + carry to reg a.
 * A = A + (R1 + CY) */
uint8_t Cpu::ADC_A_n(uint8_t * reg)
{
  uint8_t val = *reg + GetFlag(CARRY);
  SetAddFlags(a, val);
  a += val;
  return 4;
}

uint8_t Cpu::ADC_A_A() { return ADC_A_n(&a); }
uint8_t Cpu::ADC_A_B() { return ADC_A_n(&b); }
uint8_t Cpu::ADC_A_C() { return ADC_A_n(&c); }
uint8_t Cpu::ADC_A_D() { return ADC_A_n(&d); }
uint8_t Cpu::ADC_A_E() { return ADC_A_n(&e); }
uint8_t Cpu::ADC_A_H() { return ADC_A_n(&h); }
uint8_t Cpu::ADC_A_L() { return ADC_A_n(&l); }

/* ADC A, (HL)
 * Add value of (HL) + carry to A. 
 * A = A + ((HL) + CY) */
uint8_t Cpu::ADC_A_atHL() {
  uint16_t address = hl();
  uint8_t val = bus->read(address) + GetFlag(CARRY);
  SetAddFlags(a, val);
  a += val;
  return 8;
}

/* ADC A, u8
 * Add u8 + carry to A. 
 * A = A + (u8 + CY) */
uint8_t Cpu::ADC_A_u8() {
  uint8_t u8 = bus->read(pc++);
  uint8_t val = u8 + GetFlag(CARRY);
  SetAddFlags(a, val);
  a += val;
  return 4;
}

/*  CP R1
 *  Compare A with R1.
 *  To compare, the Game Boy subtracts A - R1, with the result 
 *  thrown away. If (A-R1) == 0 they are equal. */
uint8_t Cpu::CP_A_n(uint8_t *reg)
{
  SetSubFlags(a, *reg);
  return 4;
}

uint8_t Cpu::CP_A_A() { return CP_A_n(&a); }
uint8_t Cpu::CP_A_B() { return CP_A_n(&b); }
uint8_t Cpu::CP_A_C() { return CP_A_n(&c); }
uint8_t Cpu::CP_A_D() { return CP_A_n(&d); }
uint8_t Cpu::CP_A_E() { return CP_A_n(&e); }
uint8_t Cpu::CP_A_H() { return CP_A_n(&h); }
uint8_t Cpu::CP_A_L() { return CP_A_n(&l); }

/*  BE: CP (HL)
 *  Compare A with (HL).
 *  To compare, the Game Boy subtracts A - (HL), with the result 
 *  thrown away. If (A-(HL)) == 0 they are equal. */
uint8_t Cpu::CP_A_atHL() {
  uint8_t val = bus->read(hl());
  SetSubFlags(a, val);
  return 8;
}

/*  FE: CP n
 *  Compare A with n.
 *  To compare, the Game Boy subtracts A - n, with the result 
 *  thrown away. If (A-n) == 0 they are equal. */
uint8_t Cpu::CP_A_u8() {
  uint8_t val = bus->read(++pc);
  SetSubFlags(a, val);
  return 8;
}

/* ========== 16-BIT ALU ========== */

/* DEC_nm
 * Decrement a given register pair.
 * This DOES NOT affect any flags. */
uint8_t Cpu::DEC_nm(uint8_t * upper_reg, uint8_t * lower_reg)
{
  uint16_t val = (*upper_reg << 8) | *lower_reg;
  --val;
  *upper_reg = val >> 8;
  *lower_reg = val & 0xFF;
  return 8;
}

uint8_t Cpu::DEC_BC() { return DEC_nm(&b, &c); }
uint8_t Cpu::DEC_DE() { return DEC_nm(&b, &c); }
uint8_t Cpu::DEC_HL() { return DEC_nm(&b, &c); }

uint8_t Cpu::DEC_SP() {
  --sp;
  SetSubFlags(sp, 1);
  return 8;
}

/* INC_nm
 * Increment a given register pair.
 * Apparently this DOES NOT affect any flags. */
uint8_t Cpu::INC_nm(uint8_t * upper_reg, uint8_t * lower_reg)
{
  uint16_t val = (*upper_reg << 8) | *lower_reg;
  ++val;
  *upper_reg = val >> 8;
  *lower_reg = val & 0xFF;
  return 8;
}

uint8_t Cpu::INC_BC() { return INC_nm(&b, &c); }
uint8_t Cpu::INC_DE() { return INC_nm(&d, &e); }
uint8_t Cpu::INC_HL() { return INC_nm(&h, &l); }

uint8_t Cpu::INC_SP() {
  SetAddFlags(sp, 1);
  ++sp;
  return 8;
}

/* 09: Add the contents of register pair nm to the contents of register pair HL,
 * and store the results in register pair HL. */
uint8_t Cpu::ADD_HL_nm(uint8_t * upper, uint8_t * lower)
{
  uint16_t regpair = (*upper << 8) | *lower;
  uint16_t sum = hl() + regpair;
  SetAddFlags(hl(), regpair);
  h = sum >> 8;
  l = sum & 0xFF;
  AssignFlag(SUB, 0);
  return 8;
}

uint8_t Cpu::ADD_HL_BC() { return ADD_HL_nm(&b, &c); }
uint8_t Cpu::ADD_HL_DE() { return ADD_HL_nm(&d, &e); }
uint8_t Cpu::ADD_HL_HL() { return ADD_HL_nm(&h, &l); }

uint8_t Cpu::ADD_HL_SP() {
  uint16_t sum = hl() + sp;
  SetAddFlags(hl(), sp);
  h = sum >> 8;
  l = sum & 0xFF;
  AssignFlag(SUB, 0);
  return 8;
}

/* E8: ADD SP, i8
 * Add n to sp */
uint8_t Cpu::ADD_SP_i8() {
  int8_t i8 = bus->read(pc++);
  SetHalfCarryAdd(sp, i8);
  SetCarryAdd(sp, i8);
  sp += i8;
  AssignFlag(ZERO, 0);
  AssignFlag(SUB, 0);
  return 16;
}
