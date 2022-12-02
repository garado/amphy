
#include <iomanip>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "cpu.h"
#include "../utils/debug.h"

/* Cpu::AssignFlag
 * Assign an f-register flag to a value. */
void Cpu::AssignFlag(CpuFlags flag, bool val) {
  f &= ~(1 << flag);    // set bit to 0
  f |= (val << flag);   // so we can set to val
}

bool Cpu::GetFlag(CpuFlags flag) {
  return (f >> flag) & 0x1;
}


/* █▀█ ▄▄ █▄▄ █ ▀█▀ */ 
/* ███ ░░ █▄█ █ ░█░ */

/* Cpu::SetHalfCarryAdd
 * Set half carry flag for addition. */
void Cpu::SetHalfCarryAdd(uint8_t a, uint8_t b) {
  bool val = ((a & 0xF) + (b & 0xF)) > 0x10;
  AssignFlag(HALF_CARRY, val);
}

/* Cpu::SetCarryAdd
 * Set carry flag for addition. */
void Cpu::SetCarryAdd(uint8_t a, uint8_t b) {
  bool val =  (int) (a + b) > 255;
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

/*  Cpu::SetAddFlags
 *  Sets zero, sub, half carry, and carry flags for addition */
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

// ----------------------------

/* ▄█ █▄▄ ▄▄ █▄▄ █ ▀█▀ */ 
/* ░█ █▄█ ░░ █▄█ █ ░█░ */ 

/* Cpu::SetHalfCarryAdd
 * Set half carry flag for addition. */
void Cpu::SetHalfCarryAdd(uint16_t a, uint16_t b) {
  bool val = ((a & 0xF) + (b & 0xF)) > 0xF;
  AssignFlag(HALF_CARRY, val);
}

/* Cpu::SetCarryAdd
 * Set carry flag for addition. */
void Cpu::SetCarryAdd(uint16_t a, uint16_t b) {
  bool val =  (int) (a + b) > 255;
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

// 0 - 90

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
bool Cpu::execute() {
  if (!cpuEnabled) return EXIT_SUCCESS;
  
  op = bus->read(pc);
  // std::cout << "cpu::execute - prestep" << std::endl;
  // debugger->step();
  // std::cout << "cpu::execute - poststep" << std::endl;

  // // debug: wait for keypress before advancing
  // // press enter for next cycle
  // if (numCycles == 0 && bkpt != 0) {
  //   if (pc == bkpt) {
  //     debugger();
  //   }
  // } else if (numCycles == 0) {
  //   debugger();
  // } else {
  //   --numCycles;
  // }

  // if (numCycles == 0 && bkpt == 1) {
  //   debugger(); 
  // } else {
  //   --numCycles;
  // }

  uint8_t cyclesElapsed = (this->*opcodes[op])();

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

  return EXIT_SUCCESS;
};
