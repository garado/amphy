
/* ▄▀█ █░░ █░█ */ 
/* █▀█ █▄▄ █▄█ */ 

#include "cpu.h"
#include "../defines.h"

/* ========== 8-BIT ALU ========== */

/* 34: INC HL
 * Increment contents of (HL). */
void Cpu::INC_atHL() {
  uint8_t val = bus->read( hl() );
  SetAddFlags(val, 1);
  ++val;
  bus->write(hl(), val);
  cycles_last_taken = 12;
}

/* 35: DEC HL
 * Decrement contents of (HL). */
void Cpu::DEC_atHL() {
  uint8_t val = bus->read(hl());
  SetSubFlags(val, 1);
  --val;
  bus->write(hl(), val);
  cycles_last_taken = 12;
}

/* 86: ADD A, (HL)
 * A = A + (HL) */
void Cpu::ADD_A_atHL() {
  uint16_t address = hl();
  uint8_t val = bus->read(address);
  SetAddFlags(a, val);
  a += val;
  cycles_last_taken = 8;
}

/*  C6: ADD A, u8
 *  A = A + u8 */
void Cpu::ADD_A_u8() {
  int8_t u8 = bus->read(pc++);
  SetAddFlags(a, u8);
  a += u8;
  cycles_last_taken = 8;
}

/*  96: SUB (HL)
 *  Subtract (HL) from A. */
void Cpu::SUB_A_atHL() {
  uint8_t val = bus->read( hl() );
  SetSubFlags(a, val);
  a -= val;
  cycles_last_taken = 8;
}

/*  D6: SUB n
 *  Subtract 8bit immed u8 from A */
void Cpu::SUB_A_u8() {
  uint8_t u8 = bus->read(pc++);
  SetSubFlags(a, u8);
  a -= u8;
  cycles_last_taken = 8;
}

// -------------------------------

/* INC_n
 * Increment the contents of the specified register.
 * This DOES NOT set the carry flag (so we can't use SetAddFlags) */
void Cpu::INC_n(uint8_t * reg)
{
  SetHalfCarryAdd(*reg, 1);
  AssignFlag(SUB, 0);
  *reg = *reg + 1;
  AssignFlag(ZERO, (*reg == 0));
  cycles_last_taken = 4;
}

void Cpu::INC_A() { INC_n(&a); }
void Cpu::INC_B() { INC_n(&b); }
void Cpu::INC_C() { INC_n(&c); }
void Cpu::INC_D() { INC_n(&d); }
void Cpu::INC_E() { INC_n(&e); }
void Cpu::INC_H() { INC_n(&h); }
void Cpu::INC_L() { INC_n(&l); }

/* DEC_n
 * Decrement the contents of the specified register.
 * This DOES NOT set the carry flag (so we can't use SetSubFlags) */
void Cpu::DEC_n(uint8_t * reg)
{
  AssignFlag(SUB, 1);
  SetHalfCarrySub(*reg, 1);
  *reg = *reg - 1;
  AssignFlag(ZERO, (*reg == 0));
  cycles_last_taken = 4;
}

void Cpu::DEC_A() { DEC_n(&a); }
void Cpu::DEC_B() { DEC_n(&b); }
void Cpu::DEC_C() { DEC_n(&c); }
void Cpu::DEC_D() { DEC_n(&d); }
void Cpu::DEC_E() { DEC_n(&e); }
void Cpu::DEC_H() { DEC_n(&h); }
void Cpu::DEC_L() { DEC_n(&l); }

/* ADD A, n
 * Add the contents of reg n to reg a.
 * Store the result in reg a. */
void Cpu::ADD_A_n(uint8_t * reg)
{
  SetAddFlags(a, *reg);
  a += *reg;
  cycles_last_taken = 4;
}

void Cpu::ADD_A_A() { ADD_A_n(&a); }
void Cpu::ADD_A_B() { ADD_A_n(&b); }
void Cpu::ADD_A_C() { ADD_A_n(&c); }
void Cpu::ADD_A_D() { ADD_A_n(&d); }
void Cpu::ADD_A_E() { ADD_A_n(&e); }
void Cpu::ADD_A_H() { ADD_A_n(&h); }
void Cpu::ADD_A_L() { ADD_A_n(&l); }

/* SUB A, n
 * Subtract the contents of reg n from reg a.
 * Store the result in reg a. */
void Cpu::SUB_A_n(uint8_t * reg)
{
  SetSubFlags(a, *reg);
  a -= *reg;
  cycles_last_taken = 4;
}

void Cpu::SUB_A_A() { SUB_A_n(&a); }
void Cpu::SUB_A_B() { SUB_A_n(&b); }
void Cpu::SUB_A_C() { SUB_A_n(&c); }
void Cpu::SUB_A_D() { SUB_A_n(&d); }
void Cpu::SUB_A_E() { SUB_A_n(&e); }
void Cpu::SUB_A_H() { SUB_A_n(&h); }
void Cpu::SUB_A_L() { SUB_A_n(&l); }

/* SBC A, n
 * Subtract value of reg n + carry from reg a.
 * A = A - (R1 + CY) */
void Cpu::SBC_A_n(uint8_t * reg)
{
  uint8_t val = *reg + GetFlag(CARRY);
  SetSubFlags(a, val);
  a -= val;
  cycles_last_taken = 4;
}

void Cpu::SBC_A_A() { SBC_A_n(&a); }
void Cpu::SBC_A_B() { SBC_A_n(&b); }
void Cpu::SBC_A_C() { SBC_A_n(&c); }
void Cpu::SBC_A_D() { SBC_A_n(&d); }
void Cpu::SBC_A_E() { SBC_A_n(&e); }
void Cpu::SBC_A_H() { SBC_A_n(&h); }
void Cpu::SBC_A_L() { SBC_A_n(&l); }

/*  SBC A, (HL)
 *  Subtract value of (HL) + carry from A. 
 *  A = A - ((HL) + CY) */
void Cpu::SBC_A_atHL() {
  uint8_t val = bus->read(hl()) + GetFlag(CARRY);
  SetSubFlags(a, val);
  a -= val;
  cycles_last_taken = 8;
}

/*  DE: SBC A, u8
 *  Subtract u8 + carry from A. 
 *  A = A - (u8 + CY) */
void Cpu::SBC_A_u8() {
  uint8_t u8 = bus->read(pc++);
  uint8_t val = u8 + GetFlag(CARRY);
  SetSubFlags(a, val);
  a -= val;
  cycles_last_taken = 4;
}

/* ADC A, n
 * Add value of reg n + carry to reg a.
 * A = A + (R1 + CY) */
void Cpu::ADC_A_n(uint8_t * reg)
{
  uint8_t val = *reg + GetFlag(CARRY);
  SetAddFlags(a, val);
  a += val;
  cycles_last_taken = 4;
}

void Cpu::ADC_A_A() { ADC_A_n(&a); }
void Cpu::ADC_A_B() { ADC_A_n(&b); }
void Cpu::ADC_A_C() { ADC_A_n(&c); }
void Cpu::ADC_A_D() { ADC_A_n(&d); }
void Cpu::ADC_A_E() { ADC_A_n(&e); }
void Cpu::ADC_A_H() { ADC_A_n(&h); }
void Cpu::ADC_A_L() { ADC_A_n(&l); }

/* ADC A, (HL)
 * Add value of (HL) + carry to A. 
 * A = A + ((HL) + CY) */
void Cpu::ADC_A_atHL() {
  uint16_t address = hl();
  uint8_t val = bus->read(address) + GetFlag(CARRY);
  SetAddFlags(a, val);
  a += val;
  cycles_last_taken = 8;
}

/* ADC A, u8
 * Add u8 + carry to A. 
 * A = A + (u8 + CY) */
void Cpu::ADC_A_u8() {
  uint8_t u8 = bus->read(pc++);
  uint8_t val = u8 + GetFlag(CARRY);
  SetAddFlags(a, val);
  a += val;
  cycles_last_taken = 4;
}

/*  CP R1
 *  Compare A with R1.
 *  To compare, the Game Boy subtracts A - R1, with the result 
 *  thrown away. If (A-R1) == 0 they are equal. */
void Cpu::CP_A_n(uint8_t *reg)
{
  SetSubFlags(a, *reg);
  cycles_last_taken = 4;
}

void Cpu::CP_A_A() { CP_A_n(&a); }
void Cpu::CP_A_B() { CP_A_n(&b); }
void Cpu::CP_A_C() { CP_A_n(&c); }
void Cpu::CP_A_D() { CP_A_n(&d); }
void Cpu::CP_A_E() { CP_A_n(&e); }
void Cpu::CP_A_H() { CP_A_n(&h); }
void Cpu::CP_A_L() { CP_A_n(&l); }

/*  BE: CP (HL)
 *  Compare A with (HL).
 *  To compare, the Game Boy subtracts A - (HL), with the result 
 *  thrown away. If (A-(HL)) == 0 they are equal. */
void Cpu::CP_A_atHL() {
  uint8_t val = bus->read(hl());
  SetSubFlags(a, val);
  cycles_last_taken = 8;
}

/*  FE: CP n
 *  Compare A with n.
 *  To compare, the Game Boy subtracts A - n, with the result 
 *  thrown away. If (A-n) == 0 they are equal. */
void Cpu::CP_A_u8() {
  uint8_t val = bus->read(++pc);
  SetSubFlags(a, val);
  cycles_last_taken = 8;
}

/* ========== 16-BIT ALU ========== */

/* DEC_nm
 * Decrement a given register pair.
 * This DOES NOT affect any flags. */
void Cpu::DEC_nm(uint8_t * upper_reg, uint8_t * lower_reg)
{
  uint16_t val = (*upper_reg << 8) | *lower_reg;
  --val;
  *upper_reg = val >> 8;
  *lower_reg = val & 0xFF;
  cycles_last_taken = 8;
}

void Cpu::DEC_BC() { DEC_nm(&b, &c); }
void Cpu::DEC_DE() { DEC_nm(&b, &c); }
void Cpu::DEC_HL() { DEC_nm(&b, &c); }

void Cpu::DEC_SP() {
  --sp;
  SetSubFlags(sp, 1);
  cycles_last_taken = 8;
}

/* INC_nm
 * Increment a given register pair.
 * Apparently this DOES NOT affect any flags. */
void Cpu::INC_nm(uint8_t * upper_reg, uint8_t * lower_reg)
{
  uint16_t val = (*upper_reg << 8) | *lower_reg;
  ++val;
  *upper_reg = val >> 8;
  *lower_reg = val & 0xFF;
  cycles_last_taken = 8;
}

void Cpu::INC_BC() { INC_nm(&b, &c); }
void Cpu::INC_DE() { INC_nm(&d, &e); }
void Cpu::INC_HL() { INC_nm(&h, &l); }

void Cpu::INC_SP() {
  SetAddFlags(sp, 1);
  ++sp;
  cycles_last_taken = 8;
}

/* 09: Add the contents of register pair nm to the contents of register pair HL,
 * and store the results in register pair HL. */
void Cpu::ADD_HL_nm(uint8_t * upper, uint8_t * lower)
{
  uint16_t regpair = (*upper << 8) | *lower;
  uint16_t sum = hl() + regpair;
  SetAddFlags(hl(), regpair);
  h = sum >> 8;
  l = sum & 0xFF;
  AssignFlag(SUB, 0);
  cycles_last_taken = 8;
}

void Cpu::ADD_HL_BC() { ADD_HL_nm(&b, &c); }
void Cpu::ADD_HL_DE() { ADD_HL_nm(&d, &e); }
void Cpu::ADD_HL_HL() { ADD_HL_nm(&h, &l); }

void Cpu::ADD_HL_SP() {
  uint16_t sum = hl() + sp;
  SetAddFlags(hl(), sp);
  h = sum >> 8;
  l = sum & 0xFF;
  AssignFlag(SUB, 0);
  cycles_last_taken = 8;
}

/* E8: ADD SP, i8
 * Add n to sp */
void Cpu::ADD_SP_i8() {
  int8_t i8 = bus->read(pc++);
  SetHalfCarryAdd(sp, i8);
  SetCarryAdd(sp, i8);
  sp += i8;
  AssignFlag(ZERO, 0);
  AssignFlag(SUB, 0);
  cycles_last_taken = 16;
}
