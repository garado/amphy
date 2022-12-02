
#include "cpu.h"


/* ========== 8-BIT LOADS ========== */

/*  LD n, (HL)
 *  Put value in mem addres in HL into R1. */
uint8_t Cpu::LD_A_atHL() {
  uint16_t address = (h << 8) | l;
  a = bus->read(address);
  ++pc;
  return 8;
}

uint8_t Cpu::LD_B_atHL() {
  uint16_t address = (h << 8) | l;
  b = bus->read(address);
  ++pc;
  return 8;
}

uint8_t Cpu::LD_C_atHL() {
  uint16_t address = (h << 8) | l;
  c = bus->read(address);
  ++pc;
  return 8;
}

uint8_t Cpu::LD_D_atHL() {
  uint16_t address = (h << 8) | l;
  d = bus->read(address);
  ++pc;
  return 8;
}

uint8_t Cpu::LD_E_atHL() {
  uint16_t address = (h << 8) | l;
  e = bus->read(address);
  ++pc;
  return 8;
}

uint8_t Cpu::LD_H_atHL() {
  uint16_t address = (h << 8) | l;
  h = bus->read(address);
  ++pc;
  return 8;
}

uint8_t Cpu::LD_L_atHL() {
  uint16_t address = (h << 8) | l;
  l = bus->read(address);
  ++pc;
  return 8;
}

/*  LD (HL), n
 *  Put contents of n into mem location specified by HL. */
uint8_t Cpu::LD_atHL_A() {
  uint16_t address = (h << 8) | l;
  bus->write(address, a);
  ++pc;
  return 8;
}

uint8_t Cpu::LD_atHL_B() {
  uint16_t address = (h << 8) | l;
  bus->write(address, b);
  ++pc;
  return 8;
}

uint8_t Cpu::LD_atHL_C() {
  uint16_t address = (h << 8) | l;
  bus->write(address, c);
  ++pc;
  return 8;
}

uint8_t Cpu::LD_atHL_D() {
  uint16_t address = (h << 8) | l;
  bus->write(address, d);
  ++pc;
  return 8;
}

uint8_t Cpu::LD_atHL_E() {
  uint16_t address = (h << 8) | l;
  bus->write(address, e);
  ++pc;
  return 8;
}

uint8_t Cpu::LD_atHL_H() {
  uint16_t address = (h << 8) | l;
  bus->write(address, h);
  ++pc;
  return 8;
}

uint8_t Cpu::LD_atHL_L() {
  uint16_t address = (h << 8) | l;
  bus->write(address, l);
  ++pc;
  return 8;
}

/*  LD (HL), u8
 *  Put contents of 8bit immed n into mem location specified by HL. */
uint8_t Cpu::LD_atHL_u8() {
  uint16_t address = (h << 8) | l;
  uint8_t val = bus->read(++pc);
  bus->write(address, val);
  ++pc;
  return 12;
}

/*  3E: LD n, u8
 *  Load 8bit immed value into n. */
uint8_t Cpu::LD_A_u8() {
  uint8_t u8 = bus->read(++pc);
  a = u8;
  ++pc;
  return 8;
}

uint8_t Cpu::LD_B_u8() {
  uint8_t u8 = bus->read(++pc);
  b = u8;
  ++pc;
  return 8;
}

uint8_t Cpu::LD_C_u8() {
  uint8_t u8 = bus->read(++pc);
  c = u8;
  ++pc;
  return 8;
}

uint8_t Cpu::LD_D_u8() {
  uint8_t u8 = bus->read(++pc);
  d = u8;
  ++pc;
  return 8;
}

uint8_t Cpu::LD_E_u8() {
  uint8_t u8 = bus->read(++pc);
  e = u8;
  ++pc;
  return 8;
}

uint8_t Cpu::LD_H_u8() {
  uint8_t u8 = bus->read(++pc);
  h = u8;
  ++pc;
  return 8;
}

uint8_t Cpu::LD_L_u8() {
  uint8_t u8 = bus->read(++pc);
  l = u8;
  ++pc;
  return 8;
}

/* LD A, #
 * Put value of # into A */
uint8_t Cpu::LD_A_A() {
  a = a;
  ++pc;
  return 4;
}

uint8_t Cpu::LD_A_B() {
  a = b;
  ++pc;
  return 4;
}

uint8_t Cpu::LD_A_C() {
  a = c;
  ++pc;
  return 4;
}

uint8_t Cpu::LD_A_D() {
  a = d;
  ++pc;
  return 4;
}

uint8_t Cpu::LD_A_E() {
  a = e;
  ++pc;
  return 4;
}

uint8_t Cpu::LD_A_H() {
  a = h;
  ++pc;
  return 4;
}

uint8_t Cpu::LD_A_L() {
  a = l;
  ++pc;
  return 4;
}


/* LD B, #
 * Put value of # into B */
uint8_t Cpu::LD_B_A() {
  b = a;
  ++pc;
  return 4;
}

uint8_t Cpu::LD_B_B() {
  b = b;
  ++pc;
  return 4;
}

uint8_t Cpu::LD_B_C() {
  b = c;
  ++pc;
  return 4;
}

uint8_t Cpu::LD_B_D() {
  b = d;
  ++pc;
  return 4;
}

uint8_t Cpu::LD_B_E() {
  b = e;
  ++pc;
  return 4;
}

uint8_t Cpu::LD_B_H() {
  b = h;
  ++pc;
  return 4;
}

uint8_t Cpu::LD_B_L() {
  b = l;
  ++pc;
  return 4;
}

/* LD C, #
 * Put value of # into C */
uint8_t Cpu::LD_C_A() {
  c = a;
  ++pc;
  return 4;
}

uint8_t Cpu::LD_C_B() {
  c = b;
  ++pc;
  return 4;
}

uint8_t Cpu::LD_C_C() {
  c = c;
  ++pc;
  return 4;
}

uint8_t Cpu::LD_C_D() {
  c = d;
  ++pc;
  return 4;
}

uint8_t Cpu::LD_C_E() {
  c = e;
  ++pc;
  return 4;
}

uint8_t Cpu::LD_C_H() {
  c = h;
  ++pc;
  return 4;
}

uint8_t Cpu::LD_C_L() {
  c = l;
  ++pc;
  return 4;
}

/* LD D, #
 * Put value of # into D */
uint8_t Cpu::LD_D_A() {
  d = a;
  ++pc;
  return 4;
}

uint8_t Cpu::LD_D_B() {
  d = b;
  ++pc;
  return 4;
}

uint8_t Cpu::LD_D_C() {
  d = c;
  ++pc;
  return 4;
}

uint8_t Cpu::LD_D_D() {
  d = d;
  ++pc;
  return 4;
}

uint8_t Cpu::LD_D_E() {
  d = e;
  ++pc;
  return 4;
}

uint8_t Cpu::LD_D_H() {
  d = h;
  ++pc;
  return 4;
}

uint8_t Cpu::LD_D_L() {
  d = l;
  ++pc;
  return 4;
}

/* LD E, #
 * Put value of # into E */
uint8_t Cpu::LD_E_A() {
  e = a;
  ++pc;
  return 4;
}

uint8_t Cpu::LD_E_B() {
  e = b;
  ++pc;
  return 4;
}

uint8_t Cpu::LD_E_C() {
  e = c;
  ++pc;
  return 4;
}

uint8_t Cpu::LD_E_D() {
  e = d;
  ++pc;
  return 4;
}

uint8_t Cpu::LD_E_E() {
  e = e;
  ++pc;
  return 4;
}

uint8_t Cpu::LD_E_H() {
  e = h;
  ++pc;
  return 4;
}

uint8_t Cpu::LD_E_L() {
  e = l;
  ++pc;
  return 4;
}

/* LD H, #
 * Put value of # into H */
uint8_t Cpu::LD_H_A() {
  h = a;
  ++pc;
  return 4;
}

uint8_t Cpu::LD_H_B() {
  h = b;
  ++pc;
  return 4;
}

uint8_t Cpu::LD_H_C() {
  h = c;
  ++pc;
  return 4;
}

uint8_t Cpu::LD_H_D() {
  h = d;
  ++pc;
  return 4;
}

uint8_t Cpu::LD_H_E() {
  h = e;
  ++pc;
  return 4;
}

uint8_t Cpu::LD_H_H() {
  h = h;
  ++pc;
  return 4;
}

uint8_t Cpu::LD_H_L() {
  h = l;
  ++pc;
  return 4;
}

/* LD L, #
 * Put value of # into L */
uint8_t Cpu::LD_L_A() {
  l =  a;
  ++pc;
  return 4;
}

uint8_t Cpu::LD_L_B() {
  l =  b;
  ++pc;
  return 4;
}

uint8_t Cpu::LD_L_C() {
  l =  c;
  ++pc;
  return 4;
}

uint8_t Cpu::LD_L_D() {
  l =  d;
  ++pc;
  return 4;
}

uint8_t Cpu::LD_L_E() {
  l =  e;
  ++pc;
  return 4;
}

uint8_t Cpu::LD_L_H() {
  l =  h;
  ++pc;
  return 4;
}

uint8_t Cpu::LD_L_L() {
  l =  l;
  ++pc;
  return 4;
}

/*  0A: LD A, (BC)
 *  Put value in mem address in BC into A. */
uint8_t Cpu::LD_A_atBC() {
  uint16_t address = (b << 8) | c;
  uint8_t val = bus->read(address);
  a = address;
  ++pc;
  return 8;
}

uint8_t Cpu::LD_A_atDE() {
  uint16_t address = (d << 8) | e;
  uint8_t val = bus->read(address);
  a = address;
  ++pc;
  return 8;
}

/*  E0: LDH (n), A
 *  Put A into address $FF00 + n; */
uint8_t Cpu::LDH_atu8_A() {
  uint8_t n = bus->read(++pc);
  uint16_t address = 0xFF00 + n;
  bus->write(address, a);
  ++pc;
  return 12;
}

/*  F0: LDH A, (n)
 *  Put value at address $FF00 + n into A. */
uint8_t Cpu::LDH_A_atu8() {
  uint8_t n = bus->read(++pc);
  uint16_t address = 0xFF00 + n;
  a = bus->read(address);
  ++pc;
  return 12;
}

/*  F2: LD A, (C)
 *  Put value at address $FF00 + RegC into A. */
uint8_t Cpu::LDH_A_atC() {
  uint16_t address = 0xFF00 | c;
  a = bus->read(address);
  ++pc;
  return 8;
}

/*  E2: LD (C), A
 *  Put A in address $FF00 + RegC. */
uint8_t Cpu::LDH_atC_A() {
  uint16_t address = 0xFF00 | c;
  bus->write(address, a);
  ++pc;
  return 8;
}

/*  3A: LDD A, (HL)
 *  Put val at address HL into A. Decrement HL. */
uint8_t Cpu::LDD_A_atHL() {
  uint16_t address = (h << 8) | l;
  a = bus->read(address);

  // Decrement HL
  address--;
  h = address >> 8;
  l = (address & 0x00FF);
  ++pc;
  return 8;
}

/*  32: LDD (HL), A
 *  Put A into address HL. Decrement HL. */
uint8_t Cpu::LDD_atHL_A() {
  uint16_t address = (h << 8) | l;
  // std::cout << "LDD (HL), A writing to: " << std::hex
      //   << (int) address << std::endl;
  bus->write(address, a);

  // Decrement HL
  address--;
  h = address >> 8;
  l = (address & 0x00FF);
  ++pc;
  return 8;
}


/*  2A: LDI A, (HL)
 *  Put val at address HL into A. Increment HL. */
uint8_t Cpu::LDI_A_atHL() {
  uint16_t address = (h << 8) | l;
  a = bus->read(address);

  // Increment HL
  address++;
  h = address >> 8;
  l = (address & 0x00FF);
  ++pc;
  return 8;
}

/*  22: LDI (HL), A
 *  Put A into address HL. Decrement HL. */
uint8_t Cpu::LDI_atHL_A() {
  uint16_t address = (h << 8) | l;
  bus->write(address, a);

  // Increment HL
  address++;
  h = address >> 8;
  l = (address & 0x00FF);
  ++pc;
  return 8;
}

/*  F8: LDHL SP, n
 *  Put SP + n effective address into HL. */
uint8_t Cpu::LD_HL_SP_i8() {
  int8_t n = bus->read(++pc);
  uint16_t newHL = sp + n;
  h = (newHL & 0xFF00) >> 8;
  l = newHL & 0xFF;
  SetAddFlags(sp, n);
  AssignFlag(ZERO, 0);
  ++pc;
  return 12;
}

/* ========== 16-BIT LOADS ========== */

/*  FA: LD A, (nn)
 *  Load value in mem address specified by 16-bit immediate nn into A. */
uint8_t Cpu::LD_A_atu16() {
  uint8_t lsb = bus->read(++pc);
  uint8_t msb = bus->read(++pc);
  uint16_t address = (msb << 8) | lsb;
  a = bus->read(address);
  ++pc;
  return 16;
}

/*  EA: LD (nn), A
 *  Store contents of register A in reg or internal RAM specified by nn */
uint8_t Cpu::LD_atu16_A() {
  uint8_t lsb = bus->read(++pc);
  uint8_t msb = bus->read(++pc);
  uint16_t address = (msb << 8) | lsb;
  bus->write(address, a);
  ++pc;
  return 16;
}

/*  01: LD BC, d16
 *  Put immed val d16 into register pair BC */
uint8_t Cpu::LD_BC_u16() {
  c = bus->read(++pc); // LSB of nn
  b = bus->read(++pc); // MSB of nn
  ++pc;
  return 12;
}

uint8_t Cpu::LD_DE_u16() {
  e = bus->read(++pc); // LSB of nn
  d = bus->read(++pc); // MSB of nn
  ++pc;
  return 12;
}

/* 31: LD SP, nn
 * Put immed val nn into SP */
uint8_t Cpu::LD_SP_u16() {
  uint8_t lsb = bus->read(++pc);
  uint8_t msb = bus->read(++pc);
  sp = (msb << 8) | lsb;
  ++pc;
  return 12;
}

uint8_t Cpu::LD_HL_u16() {
  l = bus->read(++pc); // LSB of nn
  h = bus->read(++pc); // MSB of nn
  ++pc;
  return 12;
}

/*  02: LD (BC), A
 *  Store contents of A into mem location specified by BC. */
uint8_t Cpu::LD_atBC_A() {
  uint16_t address = (b << 8) | c;
  bus->write(address, a);
  ++pc;
  return 8;
}

uint8_t Cpu::LD_atDE_A() {
  uint16_t address = (d << 8) | e;
  bus->write(address, a);
  ++pc;
  return 8;
}

/*  08: LD (U16), SP
 *  Put SP at address U16.
 *  LSB of SP goes to pc+1, and MSB goes to pc+2. */
uint8_t Cpu::LD_atu16_SP() {
  uint8_t msb = sp >> 8;
  uint8_t lsb = sp & 0xFF;
  bus->write(++pc, lsb);
  bus->write(++pc, msb);
  ++pc;
  return 20;
}

/*  F9: LD SP, HL
 *  Put HL into SP. */
uint8_t Cpu::LD_SP_HL() {
  sp = (h << 8) | l;
  ++pc;
  return 8;
}

/*  Opcodes: F5 C5 D5 E5
 *  Push register pair onto stack. Decrement SP twice. */
uint8_t Cpu::PUSH_AF() {
  // !!! check for overflow later.
  // CANNOT for the life of me find the stack range.
  bus->write(--sp, a);
  bus->write(--sp, f);
  ++pc;
  return 16;
}

uint8_t Cpu::PUSH_BC() {
  // !!! check for overflow later.
  // CANNOT for the life of me find the stack range.
  bus->write(--sp, b);
  bus->write(--sp, c);
  ++pc;
  return 16;
}

uint8_t Cpu::PUSH_DE() {
  // !!! check for overflow later.
  // CANNOT for the life of me find the stack range.
  bus->write(--sp, d);
  bus->write(--sp, e);
  ++pc;
  return 16;
}

uint8_t Cpu::PUSH_HL() {
  // !!! check for overflow later.
  // CANNOT for the life of me find the stack range.
  bus->write(--sp, h);
  bus->write(--sp, l);
  ++pc;
  return 16;
}

/*  Opcodes: F1 C1 D1 E1
 *  Pop SP to register pair R1R2. Increment SP twice. */
uint8_t Cpu::POP_AF() {
  // check for underflow later
  f = bus->read(sp++);
  a = bus->read(sp++);
  ++pc;
  return 16;
}

uint8_t Cpu::POP_BC() {
  // check for underflow later
  b = bus->read(sp++);
  c = bus->read(sp++);
  ++pc;
  return 16;
}

uint8_t Cpu::POP_DE() {
  // check for underflow later
  d = bus->read(sp++);
  e = bus->read(sp++);
  ++pc;
  return 16;
}

uint8_t Cpu::POP_HL() {
  // check for underflow later
  h = bus->read(sp++);
  l = bus->read(sp++);
  ++pc;
  return 16;
}
