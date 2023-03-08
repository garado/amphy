
/* ▄█ █▄▄ ▄▄ █▄▄ █ ▀█▀    █▀█ █▀█ █▀▀ █▀█ █▀▄ █▀▀ █▀ */
/* ░█ █▄█ ░░ █▄█ █ ░█░    █▄█ █▀▀ █▄▄ █▄█ █▄▀ ██▄ ▄█ */ 

#include "cpu.h"
#include <stdio.h>
#include <iostream>
#include <array>
#include <fstream>
#include <vector>

#include "../bus.h"

/* @Function  Cpu::Decode16bitReg 
 * @param     opcode The opcode
 * @return    Pointer to the register or value to be operated on
 * @brief     The 16-bit opcode table follows a very specific pattern.
 *            The lower byte tells you which register (or value)
 *            the instruction operates on. */
uint8_t * Cpu::Decode16bitReg(uint8_t opcode)
{
  uint8_t lsb = opcode & 0xF;
  switch (lsb) {
  case 0x00:  return &b; break;
  case 0x01:  return &c; break;
  case 0x02:  return &d; break;
  case 0x03:  return &e; break;
  case 0x04:  return &h; break;
  case 0x05:  return &l; break;
  case 0x06:  return bus->GetAddressPointer(hl()); break; // $hl
  case 0x07:  return &a; break;
  case 0x08:  return &b; break;
  case 0x09:  return &c; break;
  case 0x0a:  return &d; break;
  case 0x0b:  return &e; break;
  case 0x0c:  return &h; break;
  case 0x0d:  return &l; break;
  case 0x0e:  return bus->GetAddressPointer(hl()); break; // $hl
  case 0x0f:  return &a; break;
  default:    return NULL; break;
  }
}

/* @Function  Cpu::Decode16bitReg 
 * @param     opcode The opcode
 * @return    A bit position 0-7
 * @brief     Returns a bit position for the BIT, SET, and RES instructions to operate on. */
uint8_t Cpu::Decode16bitBitPos(uint8_t opcode)
{
  uint8_t msb = (opcode >> 4) & 0xF;
  uint8_t lsb = opcode & 0xF;

  switch (msb) {
  case 0x4: // bit
  case 0x8: // res
  case 0xC: // set
    if (lsb < 0x9)  return 0;
    if (lsb >= 0x9) return 1;
    break;

  case 0x5:
  case 0x9:
  case 0xD:
    if (lsb < 0x9)  return 2;
    if (lsb >= 0x9) return 3;
    break; 
  
  case 0x6:
  case 0xA:
  case 0xE:
    if (lsb < 0x9)  return 4;
    if (lsb >= 0x9) return 5;
    break;

  case 0x7:
  case 0xB:
  case 0xF:
    if (lsb < 0x9)  return 6;
    if (lsb >= 0x9) return 7;
    break;

  default: break;
  }

  return 0;
}

/* @Function  Cpu::Decode16bitOpcode
 * @param     opcode  lower byte of a 16-bit opcode
 * @brief     The 16bit opcode table is much more formulaic than the 8bit opcode table.
 * @return    none */ 
void Cpu::Decode16bitOpcode(uint8_t opcode)
{
  uint8_t msb = (opcode >> 4) & 0xF;
  uint8_t lsb = opcode & 0xF;

  uint8_t * reg = Decode16bitReg(opcode);

  if (reg == NULL) {
    std::cout << __PRETTY_FUNCTION__ << ": NULL pointer!" << std::endl;
    std::cout << "PC: 0x" << std::hex << (int) pc << std::endl;
    std::cout << "Opcode: 0xCB" << std::hex << (int) opcode << std::endl;
    //debugger->Regdump();
    exit(EXIT_FAILURE);
  }

  uint8_t bitpos = Decode16bitBitPos(opcode);

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

/* @Function  Cpu::RR
 * @param     reg   Pointer to value to rotate 
 * @brief     Rotate contents of reg to the right through CY. The previous contents
 *            of the carry flag are copied to bit 7 of the register.
 *            The previous bit 0 of the register is copied to the carry flag.
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

/* @Function  Cpu::SWAP
 * @param     reg   Pointer to value to shift
 * @brief     Shift the contents of the lower-order four bits (0-3) of reg to the higher-order four bits (4-7) of reg.
 *            Shift the higher-order four bits to the lower-order four bits. */
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

/* @Function  Cpu::BIT_n
 * @param     reg Pointer to the reg
 * @param     bitpos The bit position
 * @brief     Copy the complement of the contents of bit #bitpos in reg to Z flag */
void Cpu::BIT_n(uint8_t * reg, uint8_t bitpos)
{
  uint8_t val = (*reg >> bitpos) & 0x1;
  AssignFlag(ZERO, ~val);
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 1);
  cycles_last_taken = 8;
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
