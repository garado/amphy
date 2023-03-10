
/* █▀█ █▀█ ▀█▀ ▄▀█ ▀█▀ █▀▀ █▀    ▄▀█ █▄░█ █▀▄    █▀ █░█ █ █▀▀ ▀█▀ █▀ */ 
/* █▀▄ █▄█ ░█░ █▀█ ░█░ ██▄ ▄█    █▀█ █░▀█ █▄▀    ▄█ █▀█ █ █▀░ ░█░ ▄█ */ 

#include "cpu.h"

/* 07: RLCA
 * Rotate the contents of register A to the left. 
 * The contents of bit 7 are placed in both the CY flag and bit 0 of register A. */
void Cpu::RLCA() {
  uint8_t oldbit7 = a >> 7;
  a <<= 1;
  a |= oldbit7;
  AssignFlag(ZERO, 0);
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 0);
  AssignFlag(CARRY, oldbit7);
  cycles_last_taken = 4;
}

/* 0F: RRCA
 * Rotate A right. Old bit 0 to CY and bit 7 of A. */
void Cpu::RRCA() {
  uint8_t oldbit0 = a & 0x1;
  a >>= 1;
  a |= (oldbit0 << 7);
  AssignFlag(ZERO, 0);
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 0);
  AssignFlag(CARRY, oldbit0);
  cycles_last_taken = 4;
}

/* 17: RLA
 * Combine CY and A to make a 9-bit number with CY as msb.
 * Then rotate that left. Old CY gets copied to bit 0.
 * Old bit 8 of A is discarded. */
void Cpu::RLA() {
  uint8_t oldCy = GetFlag(CARRY);
  uint8_t oldbit7 = a >> 7;
  a <<= 1;
  a |= oldCy;
  AssignFlag(ZERO, 0);
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 0);
  AssignFlag(CARRY, oldbit7);
  cycles_last_taken = 4;
}

/* 1F: RRA
 * Combine CY and A to make a 9-bit number, with CY as msb.
 * Then rotate that right. Old CY gets copied to bit 7.
 * Old bit 0 of A is discarded. */
void Cpu::RRA() {
  uint8_t oldCy = GetFlag(CARRY);
  uint8_t oldbit0 = a & 0x1;
  a >>= 1;
  a |= (oldCy << 7);
  AssignFlag(ZERO, 0);
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 0);
  AssignFlag(CARRY, oldbit0);
  cycles_last_taken = 4;
}
