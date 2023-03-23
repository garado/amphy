
/* ▄█ █▄▄ ▄▄ █▄▄ █ ▀█▀    █▀█ █▀█ █▀▀ █▀█ █▀▄ █▀▀ █▀ */
/* ░█ █▄█ ░░ █▄█ █ ░█░    █▄█ █▀▀ █▄▄ █▄█ █▄▀ ██▄ ▄█ */ 

#include "cpu.h"
#include <stdio.h>
#include <iostream>
#include <array>
#include <fstream>
#include <vector>

#include "../bus.h"

/* @Function  Cpu::Decode16bitOpcode
 * @param     op lower byte of a 16-bit opcode
 * @brief     The 16bit opcode table is much more formulaic than the 8bit opcode table.
 * @return    none */ 
void Cpu::Decode16bitOpcode(uint8_t op)
{
  uint8_t msb = (op >> 4) & 0xF;
  uint8_t lsb = op & 0xF;

  uint8_t * reg;

  // Decoding which register it operates on
  uint8_t rlsb = (lsb + 1) & 0xF;
  if (rlsb >= 0x8) rlsb -= 0x8;
  
  switch (rlsb) {
    case 0x0:  reg = &a; break;
    case 0x1:  reg = &b; break;
    case 0x2:  reg = &c; break;
    case 0x3:  reg = &d; break;
    case 0x4:  reg = &e; break;
    case 0x5:  reg = &h; break;
    case 0x6:  reg = &l; break;
    case 0x7:  reg = bus->GetAddressPointer(hl()); break;
    default:   reg = NULL; break;
  }

  // Decoding bit position
  uint8_t bitpos = ((op & 0x30) >> 3) | ((op & 0x08) >> 3);

  switch (msb) {
  case 0x0: 
    if (lsb < 0x8)  RLC(reg); 
    if (lsb >= 0x8) RRC(reg);
    break;

  case 0x1:
    if (lsb < 0x8)  RL(reg); 
    if (lsb >= 0x8) RR(reg);
    break; 
  
  case 0x2:
    if (lsb < 0x8)  SLA(reg); 
    if (lsb >= 0x8) SRA(reg);
    break;

  case 0x3:
    if (lsb < 0x8)  SWAP(reg); 
    if (lsb >= 0x8) SRL(reg);
    break;

  case 0x4:
  case 0x5:
  case 0x6:
  case 0x7:
    BIT_n(reg, bitpos);
    break;

  case 0x8:
  case 0x9:
  case 0xa:
  case 0xb:
    RES_n(reg, bitpos);
    break;

  case 0xc:
  case 0xd:
  case 0xe:
  case 0xf:
    SET_n(reg, bitpos);
    break;

  default:   break; 
  }
}

/* @Function  Cpu::RLC
 * @param     reg   Pointer to value to rotate 
 * @brief     Rotate contents of reg to the left. Contents of bit 7 are placed in
 *            both the CY flag and bit 0 of reg.
 * @return    none */ 
void Cpu::RLC(uint8_t * reg) {
  uint8_t oldbit7 = *reg >> 7;
  *reg <<= 1;
  *reg |= oldbit7;
  AssignFlag(CARRY, oldbit7);
  AssignFlag(ZERO, *reg == 0);
  AssignFlag(HALF_CARRY, 0);
  AssignFlag(SUB, 0);
  cycles_last_taken = 8;
}

/* @Function  Cpu::RLC_atHL
 * @brief     Rotate contents of memory in address HL to the left.
 * @return    none */ 
void Cpu::RLC_atHL() {
  uint8_t val = bus->read(hl());
  uint8_t oldbit7 = val >> 7;
  val <<= 1;
  val |= oldbit7;
  AssignFlag(CARRY, oldbit7);
  AssignFlag(ZERO, val == 0);
  AssignFlag(HALF_CARRY, 0);
  AssignFlag(SUB, 0);
  bus->write(hl(), val);
  cycles_last_taken = 16;
}

/* @Function  Cpu::RRC
 * @param     reg   Pointer to value to rotate 
 * @brief     Rotate contents of reg to the right. Contents of bit 0 are placed in
 *            both the CY flag and bit 7 of reg.
 * @return    none */ 
void Cpu::RRC(uint8_t * reg) {
  uint8_t oldbit0 = *reg & 0x1;
  *reg >>= 1;
  *reg |= oldbit0 << 7;
  AssignFlag(CARRY, oldbit0);
  AssignFlag(ZERO, *reg == 0);
  AssignFlag(HALF_CARRY, 0);
  AssignFlag(SUB, 0);
  cycles_last_taken = 8;
}

/* @Function  Cpu::RRC_atHL
 * @brief     Rotate contents of value at address HL to the right.
 *            Contents of bit 0 are placed in both the CY flag and bit 7 of val.
 * @return    none */ 
void Cpu::RRC_atHL() {
  uint8_t val = bus->read(hl());
  uint8_t oldbit0 = val & 0x1;
  val >>= 1;
  val |= oldbit0 << 7;
  AssignFlag(CARRY, oldbit0);
  AssignFlag(ZERO, val == 0);
  AssignFlag(HALF_CARRY, 0);
  AssignFlag(SUB, 0);
  bus->write(hl(), val);
  cycles_last_taken = 16;
}

/* @Function  Cpu::RL
 * @param     reg   Pointer to value to rotate 
 * @brief     Rotate contents of reg to the left through CY. The previous contents
 *            of the carry flag are copied to bit 0 of the register.
 *            The previous bit 7 of the register is copied to the carry flag.
 * @return    none */ 
void Cpu::RL(uint8_t * reg) {
  uint8_t oldbit7 = *reg >> 7;
  *reg <<= 1;
  *reg |= GetFlag(CARRY);
  AssignFlag(CARRY, oldbit7);
  AssignFlag(ZERO, *reg == 0);
  AssignFlag(HALF_CARRY, 0);
  AssignFlag(SUB, 0);
  cycles_last_taken = 8;
}

/* @Function  Cpu::RL_atHL
 * @brief     Rotate contents of memory at HL to the left through CY.
 *            The previous contents of the carry flag are copied to
 *            bit 0 of value. The previous bit 7 of the value is
 *            copied to the carry flag.
 * @return    none */ 
void Cpu::RL_atHL() {
  uint8_t val = bus->read(hl());
  uint8_t oldbit7 = val >> 7;
  val <<= 1;
  val |= GetFlag(CARRY);
  AssignFlag(CARRY, oldbit7);
  AssignFlag(ZERO, val == 0);
  AssignFlag(HALF_CARRY, 0);
  AssignFlag(SUB, 0);
  bus->write(hl(), val);
  cycles_last_taken = 16;
}

/* @Function  Cpu::RR
 * @param     reg   Pointer to value to rotate 
 * @brief     Rotate contents of reg to the right through CY. The
 *            previous contents of the carry flag are copied to bit
 *            7 of the register. The previous bit 0 of the register
 *            is copied to the carry flag.
 * @return    none */ 
void Cpu::RR(uint8_t * reg) {
  uint8_t oldbit0 = *reg & 0x1;
  *reg >>= 1;
  *reg |= (GetFlag(CARRY) << 7);
  AssignFlag(CARRY, oldbit0);
  AssignFlag(ZERO, *reg == 0);
  AssignFlag(HALF_CARRY, 0);
  AssignFlag(SUB, 0);
  cycles_last_taken = 8;
}

/* @Function  Cpu::RR_atHL
 * @brief     Rotate contents of memory at HL to the right through
 *            CY. The previous contents of the carry flag are copied
 *            to bit 0 of value. The previous bit 7 of the value is
 *            copied to the carry flag.
 * @return    none */ 
void Cpu::RR_atHL() {
  uint8_t val = bus->read(hl());
  uint8_t oldbit0 = val & 0x1;
  val >>= 1;
  val |= (GetFlag(CARRY) << 7);
  AssignFlag(CARRY, oldbit0);
  AssignFlag(ZERO, val == 0);
  AssignFlag(HALF_CARRY, 0);
  AssignFlag(SUB, 0);
  bus->write(hl(), val);
  cycles_last_taken = 16;
}

/* @Function  Cpu::SLA
 * @param     reg   Pointer to value to shift
 * @brief     Shift the contents of reg to the left through CY.
 *            Bit 0 is reset to 0.
 * @return    none */ 
void Cpu::SLA(uint8_t * reg)
{
  uint8_t bit7 = *reg >> 7;
  AssignFlag(CARRY, bit7);
  *reg <<= 1;
  *reg &= 0xFE;
  AssignFlag(ZERO, *reg == 0);
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 0);
  cycles_last_taken = 8;
}

/* @Function  Cpu::SLA_atHL
 * @brief     Shift the contents of value at address HL to the
 *            left through CY. Bit 0 is reset to 0.
 * @return    none */ 
void Cpu::SLA_atHL()
{
  uint8_t val = bus->read(hl());
  uint8_t bit7 = val >> 7;
  AssignFlag(CARRY, bit7);
  val <<= 1;
  val &= 0xFE;
  AssignFlag(ZERO, val == 0);
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 0);
  bus->write(hl(), val);
  cycles_last_taken = 16;
}

/* @Function  Cpu::SRA
 * @param     reg   Pointer to value to shift
 * @brief     Shift the contents of reg to the right through CY.
 *            The previous value of bit 0 is copied to CY.
 *            Bit 7 of the register REMAINS UNCHANGED.
 * @return    none */ 
void Cpu::SRA(uint8_t * reg)
{
  uint8_t oldbit7 = *reg >> 7;
  AssignFlag(CARRY, *reg & 0x1);
  *reg >>= 1;
  *reg |= (oldbit7 << 7);
  AssignFlag(ZERO, *reg == 0);
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 0);
  cycles_last_taken = 8;
}

/* @Function  Cpu::SRA_atHL
 * @brief     Shift the contents of value at address HL to the
 *            right through CY. The previous value of bit 0 is
 *            copied to CY. Bit 7 of the value REMAINS UNCHANGED.
 * @return    none */ 
void Cpu::SRA_atHL()
{
  uint8_t val = bus->read(hl());
  uint8_t oldbit7 = val >> 7;
  AssignFlag(CARRY, val & 0x1);
  val >>= 1;
  val |= (oldbit7 << 7);
  AssignFlag(ZERO, val == 0);
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 0);
  bus->write(hl(), val);
  cycles_last_taken = 16;
}

/* @Function  Cpu::SWAP
 * @param     reg   Pointer to value to shift
 * @brief     Shift the contents of the lower-order four bits (0-3)
 *            of reg to the higher-order four bits (4-7) of reg.
 *            Shift the higher-order four bits to the lower-order
 *            four bits. */
void Cpu::SWAP(uint8_t * reg)
{
  uint8_t old_lower4 = *reg << 4;
  *reg >>= 4; // move upper 4 to lower 4
  *reg |= old_lower4;
  AssignFlag(ZERO, *reg == 0);
  AssignFlag(CARRY, 0);
  AssignFlag(HALF_CARRY, 0);
  AssignFlag(SUB, 0);
  cycles_last_taken = 8;
}

/* @Function  Cpu::SWAP_atHL
 * @brief     Shift the contents of the lower-order four bits (0-3)
 *            of value at address HLto the higher-order four bits
 *            (4-7) of reg. Shift the higher-order four bits to the
 *            lower-order four bits. */
void Cpu::SWAP_atHL()
{
  uint8_t val = bus->read(hl());
  uint8_t old_lower4 = val << 4;
  val >>= 4; // move upper 4 to lower 4
  val |= old_lower4;
  AssignFlag(ZERO, val == 0);
  AssignFlag(CARRY, 0);
  AssignFlag(HALF_CARRY, 0);
  AssignFlag(SUB, 0);
  bus->write(hl(), val);
  cycles_last_taken = 16;
}

/* @Function  Cpu::SRL
 * @param     reg   Pointer to value to shift
 * @brief     Shift contents of reg to the right through CY.
 *            Bit 7 of the reg is reset to 0. */
void Cpu::SRL(uint8_t * reg)
{
  uint8_t oldbit0 = *reg & 0x1;
  AssignFlag(CARRY, oldbit0);
  *reg >>= 1;
  *reg &= 0x7F;
  AssignFlag(ZERO, *reg == 0);
  AssignFlag(CARRY, oldbit0);
  AssignFlag(HALF_CARRY, 0);
  AssignFlag(SUB, 0);
  cycles_last_taken = 8;
}

/* @Function  Cpu::SRL_atHL
 * @brief     Shift contents of value at address HL to the
 *            right through CY. Bit 7 of val is reset to 0. */
void Cpu::SRL_atHL()
{
  uint8_t val = bus->read(hl());
  uint8_t oldbit0 = val & 0x1;
  AssignFlag(CARRY, oldbit0);
  val >>= 1;
  val &= 0x7F;
  AssignFlag(ZERO, val == 0);
  AssignFlag(CARRY, oldbit0);
  AssignFlag(HALF_CARRY, 0);
  AssignFlag(SUB, 0);
  bus->write(hl(), val);
  cycles_last_taken = 16;
}

/* @Function  Cpu::BIT_n
 * @param     reg Pointer to the reg
 * @param     bitpos The bit position
 * @brief     Copy the complement of the contents of bit n in reg to Z flag */
void Cpu::BIT_n(uint8_t * reg, uint8_t n)
{
  uint8_t val = (*reg >> n) & 0x1;
  AssignFlag(ZERO, !val);
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 1);
  cycles_last_taken = 8;
}

/* @Function  Cpu::BIT_n_atHL
 * @param     bitpos The bit position
 * @brief     Copy the complement of the contents of value at
 *            address HL to Z flag */
void Cpu::BIT_n_atHL(uint8_t n)
{
  uint8_t val = bus->read(hl());
  val = (val >> n) & 0x1;
  AssignFlag(ZERO, !val);
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 1);
  bus->write(hl(), val);
  cycles_last_taken = 16;
}

/* @Function  Cpu::RES_n
 * @param     reg Pointer to the reg
 * @param     bitpos The bit position
 * @brief     Reset bit #bitpos in reg to 0. */
void Cpu::RES_n(uint8_t * reg, uint8_t bitpos)
{
  uint8_t mask = (0xFF ^ (0x1 << bitpos));
  *reg &= mask;
  cycles_last_taken = 8;
}

/* @Function  Cpu::RES_n_atHL
 * @param     bitpos The bit position
 * @brief     Reset bit #bitpos in value at address HL to 0. */
void Cpu::RES_n_atHL(uint8_t bitpos)
{
  uint8_t val = bus->read(hl());
  uint8_t mask = (0xFF ^ (0x1 << bitpos));
  val &= mask;
  bus->write(hl(), val);
  cycles_last_taken = 16;
}

/* @Function  Cpu::SET_n
 * @param     reg Pointer to the reg
 * @param     bitpos The bit position
 * @brief     Set bit #bitpos in reg to 1. */
void Cpu::SET_n(uint8_t * reg, uint8_t bitpos)
{
  uint8_t mask = 0x1 << bitpos;
  *reg |= mask;
  cycles_last_taken = 8;
}

/* @Function  Cpu::SET_n_atHL
 * @param     bitpos The bit position
 * @brief     Set bit #bitpos in value at address HL to 1. */
void Cpu::SET_n_atHL(uint8_t bitpos)
{
  uint8_t val = bus->read(hl());
  uint8_t mask = 0x1 << bitpos;
  val |= mask;
  bus->write(hl(), val);
  cycles_last_taken = 16;
}
