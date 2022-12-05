
#include <iomanip>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "cpu.h"
#include "../utils/debug.h"


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
  bool val = (((a & 0xF) + (b & 0xF)) & 0x10) == 0x10;
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
 * For 16-bit numbers, this occurs when there is a carry from bit 7 to bit 8. */
void Cpu::SetHalfCarryAdd(uint16_t a, uint16_t b) {
  // unsure if this is correct
  bool val = (((a & 0xFF) + (b & 0xFF)) & 0x100) == 0x100;
  AssignFlag(HALF_CARRY, val);
}

/* Cpu::SetCarryAdd
 * Set carry flag for addition.
 * For 16-bit numbers, this is set when addition overflowed out of the 8th bit. */
void Cpu::SetCarryAdd(uint16_t a, uint16_t b) {
  bool val =  (int) (a + b) > 65536;
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
  bool val = (int) (a - b) < 0;
  AssignFlag(CARRY, val);
}

/*  Cpu::SetAddFlags
 *  Sets zero, sub, half carry, and carry flags for addition */
void Cpu::SetAddFlags(uint16_t a, uint16_t b) {
  uint16_t result = a + b;
  AssignFlag(ZERO, (result==0));
  AssignFlag(SUB, 0);
  SetHalfCarryAdd(a, b);
  SetCarryAdd(a, b);
}

/*  Cpu::SetSubFlags
 *  Sets zero, sub, half carry, and carry flags for subtraction */
void Cpu::SetSubFlags(uint16_t a, uint16_t b) {
  uint16_t result = a - b;
  AssignFlag(ZERO, (result==0));
  AssignFlag(SUB, 1);
  SetHalfCarrySub(a, b);
  SetCarrySub(a, b);
}

/* Cpu::execute
 * Handles execution of all opcodes.
 * Returns the number of t-cycles elapsed. */
uint8_t Cpu::execute() {
  //if (!cpuEnabled) return EXIT_SUCCESS;
  
  op = bus->read(pc);
  ++pc;

  // Index into opcode function table and execute opcode
  uint8_t cyclesElapsed;
  try {
    // I dont know how to check if function table entry is null lmao
    // So here is this
    std::string opcode_name = opcode_names[op];
    if (opcode_name == "NULL") {
      std::cout << "Cpu::Execute: Trying to access NULL opcode " << std::hex << (int) op << std::endl;
      std::cout << "Exiting..." << std::endl;
      exit(EXIT_FAILURE);
      //throw(69);
    } else {
      cyclesElapsed = (this->*opcodes[op])();
    }
  } catch (const std::exception &exc) {
    std::cerr << "Cpu::Execute: error" << std::endl;
    std::cerr << exc.what() << std::endl;

    std::cout << "Opcode 0x" << std::hex << std::setw(2) << std::setfill('0') << op <<
              ": " << opcode_names[op] << std::endl;
    std::cout << "af: " << std::hex << std::setw(4) << std::setfill('0') << af() << std::endl;
    std::cout << "bc: " << std::hex << std::setw(4) << std::setfill('0') << bc() << std::endl;
    std::cout << "de: " << std::hex << std::setw(4) << std::setfill('0') << de() << std::endl;
    std::cout << "hl: " << std::hex << std::setw(4) << std::setfill('0') << hl() << std::endl;
    std::cout << "sp: " << std::hex << std::setw(4) << std::setfill('0') << sp << std::endl;
    std::cout << "pc: " << std::hex << std::setw(4) << std::setfill('0') << pc << std::endl;
    std::cout << std::endl;
  }

  // janky di instruction implementation
  if (disableInterrupts != 0) {
    --disableInterrupts;
    if (disableInterrupts == 0) {
      ime = 0;
    }
  }

  // janky ei instruction implementation
  if (enableInterrupts != 0) {
    --enableInterrupts;
    if (enableInterrupts == 0) {
      ime = 1;
    }
  }

  return cyclesElapsed;
};

uint16_t Cpu::FetchNextTwoBytes(void)
{
  uint8_t lsb = bus->read(++pc);
  uint8_t msb = bus->read(++pc);
  return (msb << 8) | lsb;
}

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
