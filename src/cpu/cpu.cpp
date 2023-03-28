
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
#include "../ppu.h"
#include "../utils/debug.h"
#include "../defines.h"

/* @Function Cpu::tick()
 * @brief Used to synchronize CPU with other processing units.
 * The CPU runs on a 1MHz clock and the PPU/APU run on a
 * 4MHz clock. tick() needs to be called within every instruction to 
 * more precisely emulate concurrently running the PPU. */
void Cpu::tick()
{
  bool ppu_exec_result = ppu->Execute(4);
  if (ppu_exec_result == FAILURE) {
    std::cout << __PRETTY_FUNCTION__ << ": Fatal PPU error" << std::endl;
    debugger->RegdumpGBDoc();
  }
}

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

/* @Function  Cpu::handleInterrupt
 * @return    true if ISR executes, false otherwise
 * For an interrupt to occur:
 *    - an interrupt request must be received (IRQ)
 *    - the IME flag must be set
 *    - the corresponding interrupt enable bits must be set 
 * When an interrupt occurs, the IF (IRQ) bit is cleared,
 * and pc is pushed to the stack and then set to the address
 * of the interrupt handler. */
uint8_t Cpu::handleInterrupt() {
  uint8_t ie  = bus->read(INTE);
  uint8_t irq = bus->read(INTF);

  // If halted... unhalt
  if (((ie & irq) != 0) && cpuHalted) {
    cpuHalted = false;

    // If interrupt was pending as halt was first called
    // Halt bug: next byte after HALT read twice
    if (prevInterruptState) {
      op = bus->read(pc + 1);
      (this->*opcodes_8bit[op])();
      runTimer();
      halt_bug_executed = true;
      return true;
    }
  }

  if (!ime) return false;
  uint8_t ret = false;

  // Now check for and respond to various interrupts

  // VBlank
  uint8_t ie_vblank  = INT_VBLANK_MASK(ie);
  uint8_t irq_vblank = INT_VBLANK_MASK(irq);
  if (ie_vblank & irq_vblank) {
    Push16Bit(pc);
    pc = ISR_ADDR_VBLANK;
    ime_prev = ime;
    bus->write(INTF, INT_VBLANK_CLEAR(irq));
    return true;
  }

  // STAT
  uint8_t ie_stat  = INT_STAT_MASK(ie);
  uint8_t irq_stat = INT_STAT_MASK(irq);
  if (ie_stat & irq_stat) {
    Push16Bit(pc);
    pc = ISR_ADDR_STAT;
    ime_prev = ime;
    bus->write(INTF, INT_STAT_CLEAR(irq));
    return true;
  }

  // Timer
  uint8_t ie_timer  = INT_TIMER_MASK(ie);
  uint8_t irq_timer = INT_TIMER_MASK(irq);
  if (ie_timer & irq_timer) {
    Push16Bit(pc);
    pc = ISR_ADDR_TIMER;
    ime_prev = ime;
    bus->write(INTF, INT_TIMER_CLEAR(irq));
    return true;
  }

  // Serial
  uint8_t ie_serial  = INT_SERIAL_MASK(ie); 
  uint8_t irq_serial = INT_SERIAL_MASK(irq);
  if (ie_serial & irq_serial) {
    Push16Bit(pc);
    pc = ISR_ADDR_SERIAL;
    ime_prev = ime;
    bus->write(INTF, INT_SERIAL_CLEAR(irq));
    return true;
  }

  // Joypad
  uint8_t ie_joypad  = INT_JOYPAD_MASK(ie);
  uint8_t irq_joypad = INT_JOYPAD_MASK(irq);
  if (ie_joypad & irq_joypad) {
    Push16Bit(pc);
    pc = ISR_ADDR_JOYPAD;
    ime_prev = ime;
    bus->write(INTF, INT_JOYPAD_CLEAR(irq));
    return true;
  }

  return ret;
}

/* @Function Cpu::runTimer
 * @brief    Increments DIV and TIMA timers.
 * Internally there is a SYSCLK that updates every 4 t-cycles.
 * The DIV timer is the upper 8 bits of SYSCLK.
 * TIMA is enabled by and incremented at the clock frequency specified by TAC. */
void Cpu::runTimer() {
  uint16_t old_sysclk = sysclk;
  sysclk += cycles_last_taken;

  bus->allow_div = true; // TODO jank
  bus->write(DIV, sysclk >> 8);
  bus->allow_div = false;

  // TIMA is incremented at the frequency specified by TAC.
  // When the value overflows (> 0xFF), it is reset to the value
  // specified in TMA and an interrupt is requested.
  uint8_t tac = bus->read(TAC);

  // Ensure TIMA is enabled
  if (!TAC_ENABLE_MASK(tac)) return;

  // If tac_select has value TAC_1024, then it updates every
  // 1024 clock cycles. log_2(1024) = 10. We can tell if 1024
  // cycles have passed by checking if the 10th bit of sysclk
  // changes state.
  uint8_t tac_select = TAC_SELECT_MASK(tac);
  uint16_t tac_mask = 0;
  switch(tac_select){
    case TAC_1024:  tac_mask = 0x400; break;
    case TAC_16:    tac_mask = 0x10;  break;
    case TAC_64:    tac_mask = 0x40;  break;
    case TAC_256:   tac_mask = 0x100; break;
    default: break;
  }

  // XOR returns 1 if bits are different (state change). Then
  // we mask with tac_mask to check if a specific bit changes state.
  if ((old_sysclk ^ sysclk) & tac_mask) {
    uint8_t tima = bus->read(TIMA);

    // Request interrupt and reset TIMA to value in TMA if
    // overflow occurs. Increment normally otherwise.
    if (tima == 0xFF) {

      uint8_t tma = bus->read(TMA);   // Reset to TMA
      bus->write(TIMA, tma);

      uint8_t irq = bus->read(INTF);  // Set interrupt flag
      irq |= (1 << INT_TIMER_BIT);
      bus->write(INTF, irq);
    } else {
      bus->write(TIMA, ++tima);
    }
  }
}

/* @Function  Cpu::execute
 * @return    SUCCESS or FAILURE
 * @param     None
 * @brief     Handles execution of all opcodes. */
uint8_t Cpu::execute() {
  // Check for interrupts first
  if (handleInterrupt()) {
    if (halt_bug_executed) {
      halt_bug_executed = false;
      return SUCCESS;
    }
    cycles_last_taken = 20; // interrupt handler takes 20 cycles
    runTimer();
    return SUCCESS;
  }

  if (cpuHalted) {
    if (gbdoc) debugger->RegdumpGBDoc();
    runTimer();
    return SUCCESS;
  }

  if (gbdoc) debugger->RegdumpGBDoc();
  if (debug) debugger->step();
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

  runTimer();

  /* The effect of EI is delayed by 1 instruction.
   * EI_counter is a counter initialized to 2 when EI instruction
   * is called. It decrements on every instruction so that
   * it gets to 0 when the instruction after EI is executed. */
  if (EI_counter > 0) {
    --EI_counter;
    if (EI_counter == 0) {
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
