
#include "cpu.h"

/* ========== 8-BIT ALU ========== */

/* 34: INC HL
 * Increment contents of (HL). */
uint8_t Cpu::INC_atHL() {
  uint8_t val = bus->read(hl());
  SetHalfCarryAdd(val, 1);
  ++val;
  bus->write(hl(), val);
  AssignFlag(ZERO, (val==0));
  AssignFlag(SUB, 0);
  ++pc;
  return 12;
}


/* 3C: INC A
 * Increment register A. */
uint8_t Cpu::INC_A() {
  SetAddFlags(a, 1);
  ++a;
  ++pc;
  return 4;
}

/* 04: INC B
 * Increment register B. */
uint8_t Cpu::INC_B() {
  SetAddFlags(b, 1);
  ++b;
  ++pc;
  return 4;
}

/* 0C: INC C
 * Increment register C. */
uint8_t Cpu::INC_C() {
  SetAddFlags(c, 1);
  ++c;
  ++pc;
  return 4;
}

/* 14: INC D
 * Increment register D. */
uint8_t Cpu::INC_D() {
  SetAddFlags(d, 1);
  ++d;
  ++pc;
  return 4;
}

/* 0C: INC E
 * Increment register E. */
uint8_t Cpu::INC_E() {
  SetAddFlags(e, 1);
  ++e;
  ++pc;
  return 4;
}

/* 24: INC H
 * Increment register H. */
uint8_t Cpu::INC_H() {
  SetAddFlags(h, 1);
  ++c;
  ++pc;
  return 4;
}

/* 2C: INC L
 * Increment register L. */
uint8_t Cpu::INC_L() {
  SetAddFlags(l, 1);
  ++l;
  ++pc;
  return 4;
}

/* DEC #
 * Decrement register. */
uint8_t Cpu::DEC_A() {
  SetSubFlags(a, 1);
  --a;
  ++pc;
  return 4;
}

uint8_t Cpu::DEC_B() {
  SetSubFlags(b, 1);
  --b;
  ++pc;
  return 4;
}

uint8_t Cpu::DEC_C() {
  SetSubFlags(c, 1);
  --c;
  ++pc;
  return 4;
}

uint8_t Cpu::DEC_D() {
  SetSubFlags(d, 1);
  --d;
  ++pc;
  return 4;
}

uint8_t Cpu::DEC_E() {
  SetSubFlags(e, 1);
  --e;
  ++pc;
  return 4;
}

uint8_t Cpu::DEC_H() {
  SetSubFlags(h, 1);
  --h;
  ++pc;
  return 4;
}

uint8_t Cpu::DEC_L() {
  SetSubFlags(l, 1);
  --l;
  ++pc;
  return 4;
}

/* 35: DEC HL
 * Decrement contents of (HL). */
uint8_t Cpu::DEC_atHL() {
  uint8_t val = bus->read(hl());
  SetHalfCarrySub(val, 1);
  --val;
  bus->write(hl(), val);
  AssignFlag(ZERO, (val == 0));
  AssignFlag(SUB, 1);
  ++pc;
  return 12;
}

/*  ADD A, n
 *  Add n to A. */
uint8_t Cpu::ADD_A_A() {
  SetAddFlags(a, a);
  a += a;
  ++pc;
  return 4;
}

uint8_t Cpu::ADD_A_B() {
  SetAddFlags(a, b);
  a += b;
  ++pc;
  return 4;
}

uint8_t Cpu::ADD_A_C() {
  SetAddFlags(a, c);
  a += c;
  ++pc;
  return 4;
}

uint8_t Cpu::ADD_A_D() {
  SetAddFlags(a, d);
  a += d;
  ++pc;
  return 4;
}

uint8_t Cpu::ADD_A_E() {
  SetAddFlags(a, e);
  a += e;
  ++pc;
  return 4;
}

uint8_t Cpu::ADD_A_H() {
  SetAddFlags(a, h);
  a += h;
  ++pc;
  return 4;
}

uint8_t Cpu::ADD_A_L() {
  SetAddFlags(a, l);
  a += l;
  ++pc;
  return 4;
}

/*  86: ADD A, (HL)
 *  A = A + (HL) */
uint8_t Cpu::ADD_A_atHL() {
  uint16_t address = (h << 8) | l;
  uint8_t val = bus->read(address);
  SetAddFlags(a, val);
  a += val;
  ++pc;
  return 8;
}

/*  C6: ADD A, u8
 *  A = A + u8 */
uint8_t Cpu::ADD_A_u8() {
  int8_t u8 = bus->read(++pc);
  SetAddFlags(a, u8);
  a += u8;
  ++pc;
  return 8;
}

/*  SUB A, n
 *  Sub n from A. */
uint8_t Cpu::SUB_A_A() {
  SetSubFlags(a, a);
  a -= a;
  ++pc;
  return 4;
}

uint8_t Cpu::SUB_A_B() {
  SetSubFlags(a, b);
  a -= b;
  ++pc;
  return 4;
}

uint8_t Cpu::SUB_A_C() {
  SetSubFlags(a, c);
  a -= c;
  ++pc;
  return 4;
}

uint8_t Cpu::SUB_A_D() {
  SetSubFlags(a, d);
  a -= d;
  ++pc;
  return 4;
}

uint8_t Cpu::SUB_A_E() {
  SetSubFlags(a, e);
  a -= e;
  ++pc;
  return 4;
}

uint8_t Cpu::SUB_A_H() {
  SetSubFlags(a, h);
  a -= h;
  ++pc;
  return 4;
}

uint8_t Cpu::SUB_A_L() {
  SetSubFlags(a, l);
  a -= l;
  ++pc;
  return 4;
}

/*  96: SUB (HL)
 *  Subtract (HL) from A. */
uint8_t Cpu::SUB_A_atHL() {
  uint16_t address = (h << 8) | l;
  uint8_t val = bus->read(address);
  SetSubFlags(a, val);
  a -= val;
  ++pc;
  return 8;
}

/*  D6: SUB n
 *  Subtract 8bit immed u8 from A */
uint8_t Cpu::SUB_A_u8() {
  uint8_t u8 = bus->read(++pc);
  SetSubFlags(a, u8);
  a -= u8;
  ++pc;
  return 8;
}

/*  SBC A, R1
 *  Subtract value of R1 + carry from A. 
*   A = A - (R1 + CY) */
uint8_t Cpu::SBC_A_A() {
  uint8_t val = a + GetFlag(CARRY);
  SetSubFlags(a, val);
  a -= val;
  ++pc;
  return 4;
}

uint8_t Cpu::SBC_A_B() {
  uint8_t val = b + GetFlag(CARRY);
  SetSubFlags(a, val);
  a -= val;
  ++pc;
  return 4;
}

uint8_t Cpu::SBC_A_C() {
  uint8_t val = c + GetFlag(CARRY);
  SetSubFlags(a, val);
  a -= val;
  ++pc;
  return 4;
}

uint8_t Cpu::SBC_A_D() {
  uint8_t val = d + GetFlag(CARRY);
  SetSubFlags(a, val);
  a -= val;
  ++pc;
  return 4;
}

uint8_t Cpu::SBC_A_E() {
  uint8_t val = e + GetFlag(CARRY);
  SetSubFlags(a, val);
  a -= val;
  ++pc;
  return 4;
}

uint8_t Cpu::SBC_A_H() {
  uint8_t val = h + GetFlag(CARRY);
  SetSubFlags(a, val);
  a -= val;
  ++pc;
  return 4;
}

uint8_t Cpu::SBC_A_L() {
  uint8_t val = l + GetFlag(CARRY);
  SetSubFlags(a, val);
  a -= val;
  ++pc;
  return 4;
}

/*  SBC A, (HL)
 *  Subtract value of (HL) + carry from A. 
 *  A = A - ((HL) + CY) */
uint8_t Cpu::SBC_A_atHL() {
  uint16_t address = (h << 8) | l;
  uint8_t val = bus->read(address) + GetFlag(CARRY);
  SetSubFlags(a, val);
  a -= val;
  ++pc;
  return 8;
}

/*  DE: SBC A, u8
 *  Subtract u8 + carry from A. 
 *  A = A - (u8 + CY) */
uint8_t Cpu::SBC_A_u8() {
  uint8_t u8 = bus->read(++pc);
  uint8_t val = u8 + GetFlag(CARRY);
  SetSubFlags(a, val);
  a -= val;
  ++pc;
  return 4;
}

/*  ADC A, R1
 *  Add value of R1 + carry to A. 
*   A = A + (R1 + CY) */
uint8_t Cpu::ADC_A_A() {
  uint8_t val = a + GetFlag(CARRY);
  SetAddFlags(a, val);
  a += val;
  ++pc;
  return 4;
}

uint8_t Cpu::ADC_A_B() {
  uint8_t val = b + GetFlag(CARRY);
  SetAddFlags(a, val);
  a += val;
  ++pc;
  return 4;
}

uint8_t Cpu::ADC_A_C() {
  uint8_t val = c + GetFlag(CARRY);
  SetAddFlags(a, val);
  a += val;
  ++pc;
  return 4;
}

uint8_t Cpu::ADC_A_D() {
  uint8_t val = d + GetFlag(CARRY);
  SetAddFlags(a, val);
  a += val;
  ++pc;
  return 4;
}

uint8_t Cpu::ADC_A_E() {
  uint8_t val = e + GetFlag(CARRY);
  SetAddFlags(a, val);
  a += val;
  ++pc;
  return 4;
}

uint8_t Cpu::ADC_A_H() {
  uint8_t val = h + GetFlag(CARRY);
  SetAddFlags(a, val);
  a += val;
  ++pc;
  return 4;
}

uint8_t Cpu::ADC_A_L() {
  uint8_t val = l + GetFlag(CARRY);
  SetAddFlags(a, val);
  a += val;
  ++pc;
  return 4;
}

/*  ADC A, (HL)
 *  Add value of (HL) + carry to A. 
 *  A = A + ((HL) + CY) */
uint8_t Cpu::ADC_A_atHL() {
  uint16_t address = (h << 8) | l;
  uint8_t val = bus->read(address) + GetFlag(CARRY);
  SetAddFlags(a, val);
  a += val;
  ++pc;
  return 8;
}

/*  ADC A, u8
 *  Add u8 + carry to A. 
 *  A = A + (u8 + CY) */
uint8_t Cpu::ADC_A_u8() {
  uint8_t u8 = bus->read(++pc);
  uint8_t val = u8 + GetFlag(CARRY);
  SetAddFlags(a, val);
  a += val;
  ++pc;
  return 4;
}

/*  CP R1
 *  Compare A with R1.
 *  To compare, the Game Boy subtracts A - R1, with the result 
 *  thrown away. If (A-R1) == 0 they are equal. */
uint8_t Cpu::CP_A_A() {
  SetSubFlags(a, a);
  ++pc;
  return 4;
}

uint8_t Cpu::CP_A_B() {
  SetSubFlags(a, b);
  ++pc;
  return 4;
}

uint8_t Cpu::CP_A_C() {
  SetSubFlags(a, c);
  ++pc;
  return 4;
}

uint8_t Cpu::CP_A_D() {
  SetSubFlags(a, d);
  ++pc;
  return 4;
}

uint8_t Cpu::CP_A_E() {
  SetSubFlags(a, e);
  ++pc;
  return 4;
}

uint8_t Cpu::CP_A_H() {
  SetSubFlags(a, h);
  ++pc;
  return 4;
}

uint8_t Cpu::CP_A_L() {
  SetSubFlags(a, l);
  ++pc;
  return 4;
}

/*  BE: CP (HL)
 *  Compare A with (HL).
 *  To compare, the Game Boy subtracts A - (HL), with the result 
 *  thrown away. If (A-(HL)) == 0 they are equal. */
uint8_t Cpu::CP_A_atHL() {
  uint8_t val = bus->read(hl());
  SetSubFlags(a, val);
  ++pc;
  return 8;
}

/*  FE: CP n
 *  Compare A with n.
 *  To compare, the Game Boy subtracts A - n, with the result 
 *  thrown away. If (A-n) == 0 they are equal. */
uint8_t Cpu::CP_A_u8() {
  uint8_t val = bus->read(++pc);
  SetSubFlags(a, val);
  ++pc;
  return 8;
}


/* ========== 16-BIT ALU ========== */
uint8_t Cpu::DEC_BC() {
  uint16_t val = bc();
  --val;
  b = val >> 8;
  c = val & 0x00FF;
  ++pc;
  return 8;
}

uint8_t Cpu::DEC_DE() {
  uint16_t val = de();
  --val;
  d = val >> 8;
  e = val & 0x00FF;
  ++pc;
  return 8;
}

uint8_t Cpu::DEC_HL() {
  uint16_t val = hl();
  --val;
  h = val >> 8;
  l = val & 0x00FF;
  ++pc;
  return 8;
}

uint8_t Cpu::DEC_SP() {
  --sp;
  ++pc;
  return 8;
}

uint8_t Cpu::INC_BC() {
  uint16_t val = bc();
  ++val;
  b = val >> 8;
  c = val & 0x00FF;
  ++pc;
  return 8;
}

uint8_t Cpu::INC_DE() {
  uint16_t val = de();
  ++val;
  d = val >> 8;
  e = val & 0x00FF;
  ++pc;
  return 8;
}

uint8_t Cpu::INC_HL() {
  uint16_t val = hl();
  ++val;
  h = val >> 8;
  l = val & 0x00FF;
  ++pc;
  return 8;
}

uint8_t Cpu::INC_SP() {
  ++sp;
  ++pc;
  return 8;
}

/* 09: Add the contents of pair BC to the contents of register pair HL,
 * and store the results in register pair HL. */
uint8_t Cpu::ADD_HL_BC() {
  uint16_t sum = hl() + bc();
  SetAddFlags(hl(), bc());
  h = sum >> 8;
  l = sum & 0xFF;
  AssignFlag(SUB, 0);
  ++pc;
  return 8;
}

uint8_t Cpu::ADD_HL_DE() {
  uint16_t sum = hl() + de();
  SetAddFlags(hl(), de());
  h = sum >> 8;
  l = sum & 0xFF;
  AssignFlag(SUB, 0);
  ++pc;
  return 8;
}

uint8_t Cpu::ADD_HL_HL() {
  uint16_t sum = hl() + hl();
  SetAddFlags(hl(), hl());
  h = sum >> 8;
  l = sum & 0xFF;
  AssignFlag(SUB, 0);
  ++pc;
  return 8;
}

uint8_t Cpu::ADD_HL_SP() {
  uint16_t sum = hl() + sp;
  SetAddFlags(hl(), sp);
  h = sum >> 8;
  l = sum & 0xFF;
  AssignFlag(SUB, 0);
  ++pc;
  return 8;
}

/* E8: ADD SP, i8
 * Add n to sp */
uint8_t Cpu::ADD_SP_i8() {
  int8_t i8 = bus->read(++pc);
  SetHalfCarryAdd(sp, i8);
  SetCarryAdd(sp, i8);
  sp += i8;
  AssignFlag(ZERO, 0);
  AssignFlag(SUB, 0);
  ++pc;
  return 16;
}
