
/* █▀▀ █▀█ █░█ */ 
/* █▄▄ █▀▀ █▄█ */

#include <iomanip>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "cpu.h"
#include "../utils/debug.h"
#include "../defines.h"

/* @Function  Cpu::af()
 * @return    None
 * @param     af  A 16-bit number to set to af
 * @brief     Helper function for setting register values */
void Cpu::af(uint16_t af)
{
  a = (af >> 8) & 0xFF;
  f = af & 0xFF;
}

void Cpu::bc(uint16_t bc)
{
  b = (bc >> 8) & 0xFF;
  c = bc & 0xFF;
}

void Cpu::de(uint16_t de)
{
  d = (de >> 8) & 0xFF;
  e = de & 0xFF;
}

void Cpu::hl(uint16_t hl)
{
  h = (hl >> 8) & 0xFF;
  l = hl & 0xFF;
}

/* @Function  Cpu::AssignFlag()
 * @return    None
 * @param     flag  Which flag to set
 *            val   Its value
 * @brief     Assign an f-register flag to a value */
void Cpu::AssignFlag(CpuFlags flag, bool val) {
  f &= ~(1 << flag);    // set bit to 0
  f |= (val << flag);   // so we can set to val
}

/* @Function  Cpu::GetFlag()
 * @return    0 or 1
 * @param     flag  Which flag to read
 * @brief     Return the value of an f-register flag */
bool Cpu::GetFlag(CpuFlags flag) {
  return (f >> flag) & 0x1;
}

/* █▀█ ▄▄ █▄▄ █ ▀█▀ */ 
/* ███ ░░ █▄█ █ ░█░ */

/* @Function  Cpu::SetHalfCarryAdd() (8bit)
 * @return    None
 * @param     a   First number to add
 *            b   Second number to add
 * @brief     Sets the half carry flag for the addition of 2 8-bit numbers. 
 *            This occurs when there is a carry from the 3rd to the 4th bit. */
void Cpu::SetHalfCarryAdd(uint8_t a, uint8_t b) {
  uint16_t sum = (a & 0xF) + (b & 0xF);
  bool val = (sum & 0x10) == 0x10;
  AssignFlag(HALF_CARRY, val);
}

/* @Function  Cpu::SetCarryAdd() (8bit)
 * @return    None
 * @param     a   First number to add
 *            b   Second number to add
 * @brief     Sets the carry flag for the addition of 2 8-bit numbers. 
 *            This occurs when there is overflow out of the 4th bit. */
void Cpu::SetCarryAdd(uint8_t a, uint8_t b) {
  bool val = (int) (a + b) > 0xFF;
  AssignFlag(CARRY, val);
}

/* Cpu::SetHalfCarrySub
 * Set half carry flag for subtraction. 
 * With 8-bit numbers, this occurs when there is a carry between
 * bit 3 and bit 4. */
void Cpu::SetHalfCarrySub(uint8_t a, uint8_t b) {
  bool val = ((a & 0xF) - (b & 0xF)) & 0x10;
  AssignFlag(HALF_CARRY, val);
}

/* Cpu::SetCarrySub
 * Set carry flag for subtraction. */
void Cpu::SetCarrySub(uint8_t a, uint8_t b) {
  bool val = (int) (a - b) < 0;
  AssignFlag(CARRY, val);
}

/* Cpu::SetAddFlags
 * Sets zero, sub, half carry, and carry flags for addition */
void Cpu::SetAddFlags(uint8_t a, uint8_t b) {
  uint8_t result = a + b;
  AssignFlag(ZERO, (result==0));
  AssignFlag(SUB, 0);
  SetHalfCarryAdd(a, b);
  SetCarryAdd(a, b);
}

/*  Cpu::SetSubFlags
 *  Sets zero, sub, half carry, and carry flags for subtraction */
void Cpu::SetSubFlags(uint8_t a, uint8_t b) {
  uint8_t result = a - b;
  AssignFlag(ZERO, (result==0));
  AssignFlag(SUB, 1);
  SetHalfCarrySub(a, b);
  SetCarrySub(a, b);
}

/* ------------------------------------------- */

/* ▄█ █▄▄ ▄▄ █▄▄ █ ▀█▀ */ 
/* ░█ █▄█ ░░ █▄█ █ ░█░ */ 

/* Cpu::SetHalfCarryAdd
 * Set half carry flag for addition.
 * For 16-bit numbers, this occurs when there is a carry from bit 11 to bit 12. */
void Cpu::SetHalfCarryAdd(uint16_t a, uint16_t b) {
  uint16_t sum = (a & 0xFFF) + (b & 0xFFF);
  bool val = (sum & 0x1000) == 0x1000;
  AssignFlag(HALF_CARRY, val);
}

/* Cpu::SetCarryAdd
 * Set carry flag for addition.
 * For 16-bit numbers, this is set when addition overflowed out of the 8th bit. */
void Cpu::SetCarryAdd(uint16_t a, uint16_t b) {
  bool val =  ((uint32_t) a + (uint32_t) b) > 0xFFFF;
  AssignFlag(CARRY, val);
}

/* Cpu::SetHalfCarrySub
 * Set half carry flag for subtraction. */
void Cpu::SetHalfCarrySub(uint16_t a, uint16_t b) {
  bool val = (a & 0xF) + (b & 0xF) > 0xF;
  AssignFlag(HALF_CARRY, val);
}

/* Cpu::SetCarrySub
 * Set carry (borrow) flag for subtraction. 
 * This is set if the subtraction requires a borrow from the MSB. */
void Cpu::SetCarrySub(uint16_t a, uint16_t b) {
  // bool val = ((int) a - (int) b) < 0;
  bool val = b > a;
  AssignFlag(CARRY, val);
}

/*  Cpu::SetAddFlags
 *  Sets zero, sub, half carry, and carry flags for addition */
void Cpu::SetAddFlags(uint16_t a, uint16_t b, bool setzero = true) {
  uint16_t result = a + b;
  if (setzero) {
    AssignFlag(ZERO, (result==0));
  }
  AssignFlag(SUB, 0);
  SetHalfCarryAdd(a, b);
  SetCarryAdd(a, b);
}

/* @Function  Cpu::SetSubFlags
 * @param     a Number to subtract from
 * @param     b Number to subtract
 * @brief     Sets zero, sub, half carry, and carry flags for subtraction.
 *            Does a - b. */
void Cpu::SetSubFlags(uint16_t a, uint16_t b) {
  uint16_t result = a - b;
  AssignFlag(ZERO, (result==0));
  AssignFlag(SUB, 1);
  SetHalfCarrySub(a, b);
  SetCarrySub(a, b);
}

/* @Function  Cpu::execute
 * @return    SUCCESS or FAILURE
 * @param     None
 * @brief     Handles execution of all opcodes. */
uint8_t Cpu::execute() {
  op = bus->read(pc++);

  // Check for 8bit or 16bit opcode
  // 16bit opcodes are prefixed with 0xCB
  bool is16bit = (op == 0xCB);
  if (is16bit) {
    op = bus->read(pc++);
    Decode16bitOpcode(op);
  } else {
    // kind of messy - refactor later

    // Index into 8-bit opcode function table and execute opcode
    bool status;
    
    // I dont know how to check if jump table entry is null lmao
    // So here is this
    std::string opcode_name = opcode_8bit_names[op];
    if (opcode_name == "NULL") {
      std::cout << __PRETTY_FUNCTION__ << ": Trying to access NULL opcode 0x" << std::hex << (int) op << std::endl;
      debugger->RegdumpGBDoc();
      return FAILURE;
    } else {
      (this->*opcodes_8bit[op])();
    }
  }

  // Janky DI implementation
  if (disableInterrupts != 0) {
    --disableInterrupts;
    if (disableInterrupts == 0) {
      ime = 0;
    }
  }

  // Janky EI implementation
  if (enableInterrupts != 0) {
    --enableInterrupts;
    if (enableInterrupts == 0) {
      ime = 1;
    }
  }

  return SUCCESS;
};

/* █▀ ▀█▀ ▄▀█ █▀▀ █▄▀    █░█ █▀▀ █░░ █▀█ █▀▀ █▀█ █▀ */ 
/* ▄█ ░█░ █▀█ █▄▄ █░█    █▀█ ██▄ █▄▄ █▀▀ ██▄ █▀▄ ▄█ */ 

/* Cpu::Pop8Bit()
 * Pop value off the stack.
 * Read the value then increment stack pointer. */
uint8_t Cpu::Pop8Bit(void)
{
  return bus->read(sp++);
}

/* Cpu::Pop16Bit()
 * Pop value off the stack.
 * Read the value then increment stack pointer. */
uint16_t Cpu::Pop16Bit(void)
{
  uint8_t lower = bus->read(sp++);
  uint8_t upper = bus->read(sp++);
  return (upper << 8) | lower;
}

/* Cpu::Push8bit()
 * Push value to stack. 
 * Decrement the stack pointer, then write the value. */
void Cpu::Push8Bit(uint8_t val)
{
  bus->write(--sp, val);
}

/* Cpu::Push16bit()
 * Push value to stack.
 * Decrement the stack pointer, then write the value. */
void Cpu::Push16Bit(uint16_t val)
{
  uint8_t upper = val >> 8;
  uint8_t lower = val & 0xFF;
  bus->write(--sp, upper);
  bus->write(--sp, lower);
}
