
#include "cpu.h"

/* ========== JUMPS ========== */

/* E9: JP (HL)
 * Jump to address (HL). */
uint8_t Cpu::JP_HL() {
  pc = bus->read(hl());
  return 4;
}

/* 18: JR n
 * Add n to current pc. */
uint8_t Cpu::JR_i8() {
  int8_t i8 = bus->read(pc++);
  pc += i8;
  return 8;
}

/* C3: JP nn
 * Jump to address nn */
uint8_t Cpu::JP_u16() {
  uint8_t lsb = bus->read(pc++);
  uint8_t msb = bus->read(pc++);
  pc = (msb << 8) | lsb;
  return 16;
}

/* JP_flag_u16
 * Load 16bit immediate operand into pc if flag == value. 
 * Otherwise, move on to the next pc as normal. */
uint8_t Cpu::JP_flag_u16(CpuFlags flag, uint8_t value)
{
  uint8_t lsb = bus->read(pc++);
  uint8_t msb = bus->read(pc++);
  uint16_t address = (msb << 8) | lsb;

  if (GetFlag(flag) == value) {
    pc = address;
    return 16;
  } else {
    return 12;
  }
}

uint8_t Cpu::JP_NZ_u16()  { return JP_flag_u16(ZERO, 0); }
uint8_t Cpu::JP_Z_u16()   { return JP_flag_u16(ZERO, 1); }
uint8_t Cpu::JP_NC_u16()  { return JP_flag_u16(CARRY, 0); }
uint8_t Cpu::JP_C_u16()   { return JP_flag_u16(CARRY, 1); }

/* JR_flag_i8
 * Add n to current pc if flag == condition.
 * Otherwise go to next address as normal. */
uint8_t Cpu::JR_flag_i8(CpuFlags flag, uint8_t val)
{
  int8_t i8 = bus->read(pc++);

  if (GetFlag(flag) == val) {
    pc += i8;
    return 12;
  } else {
    return 8;
  }
}

uint8_t Cpu::JR_NZ_i8()  { return JR_flag_i8(ZERO, 0); }
uint8_t Cpu::JR_Z_i8()   { return JR_flag_i8(ZERO, 1); }
uint8_t Cpu::JR_NC_i8()  { return JR_flag_i8(CARRY, 0); }
uint8_t Cpu::JR_C_i8()   { return JR_flag_i8(CARRY, 1); }

/* ========== MISCELLANEOUS ========== */

/* 00: NOP
 * Does nothing. */
uint8_t Cpu::NOP() {
  return 4;
}

/* 2F: CPL
 * Complement A register. */
uint8_t Cpu::CPL() {
  AssignFlag(ZERO, 1);
  AssignFlag(HALF_CARRY, 1);
  a = ~a;
  return 4;
}

/* 3F: CCF
 * Complement carry flag. */
uint8_t Cpu::CCF() {
  uint8_t cy = GetFlag(CARRY);
  AssignFlag(CARRY, ~cy);
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 0);
  return 4;
}

/* 37: SCF
 * Set carry flag. */
uint8_t Cpu::SCF() {
  AssignFlag(CARRY, 1);
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 0);
  return 4;
}

/* 07: HALT
 * Power down CPU til interrupt occurs. */
uint8_t Cpu::HALT() {
  // !!! INTERRUPT HANDLER HAS NOT BEEN WRITTEN
  // THIS FUNCTION MAY NEED TO BE MODIFIED WHEN THAT'S DONE
  cpuEnabled = false;
  return 4;
}

/* 27: DAA
 * Tbh I do not understand wtf this does */
 uint8_t Cpu::DAA() {
  return 4;
 }

/* 10 00:STOP
 * Halt CPU and LCD until button pressed. */
uint8_t Cpu::STOP() {
  // THIS FUNCTION IS JANK
  pc += 2;
  return 4;
}

/* F3: DI
 * Disables interrupts (IME = 0) but NOT immediately.
 * Interrupts are disabled after instruction after DI is 
 * executed. */
uint8_t Cpu::DI() {
  disableInterrupts = 2; // magic number ugh
  return 4;
}

/* FB: EI
 * Enables interrupts (IME = 0) but NOT immediately.
 * Interrupts are enabled after instruction after EI is 
 * executed. */
uint8_t Cpu::EI() {
  enableInterrupts = 2; // magic number ugh
  return 4;
}

/* ========== RETURNS ========== */
/* C0: RET_NZ
 * If Z flag is 0, pop address off stack and jump to it*/
 uint8_t Cpu::RET_NZ() {
  if (GetFlag(ZERO) == 0) {
    uint8_t lsb = bus->read(sp++);
    uint8_t msb = bus->read(sp++);
    pc = (msb << 8) | lsb;
    return 20;
  } else {
    return 8;
  }
 }

 /* C8: RET_Z
 * If Z flag is 1, pop address off stack and jump to it*/
 uint8_t Cpu::RET_Z() {
  if (GetFlag(ZERO) == 1) {
    uint8_t lsb = bus->read(sp++);
    uint8_t msb = bus->read(sp++);
    pc = (msb << 8) | lsb;
    return 20;
  } else {
    return 8;
  }
 }

/* C9: RET
 * Returns from a subroutine call. Pops address off stack and
 * jumps to address. */
uint8_t Cpu::RET() {
  pc = Pop16Bit();
  return 16;
}

/* D9: RETI
 * Return from an interrupt. Sets IME flag back to its
 * pre-interrupt status. */
uint8_t Cpu::RETI() {
  uint8_t lsb = bus->read(sp++);
  uint8_t msb = bus->read(sp++);
  pc = (msb << 8) | lsb;
  ime = ime_prev;
  return 16;
}

/* D0: RET_NC
 * If Carry flag is 0, pop address off stack and jump to it*/
 uint8_t Cpu::RET_NC() {
  if (GetFlag(CARRY) == 0) {
    uint8_t lsb = bus->read(sp++);
    uint8_t msb = bus->read(sp++);
    pc = (msb << 8) | lsb;
    return 20;
  } else {
    return 8;
  }
 }

 /* D8: RET_C
 * If Z flag is 1, pop address off stack and jump to it*/
 uint8_t Cpu::RET_C() {
  if (GetFlag(CARRY) == 1) {
    uint8_t lsb = bus->read(sp++);
    uint8_t msb = bus->read(sp++);
    pc = (msb << 8) | lsb;
    return 20;
  } else {
    return 8;
  }
 }

/* ========== CALLS ========== */
/* CALL_flag_u16
 * Load 16bit immediate operand into pc if flag == value. 
 * Otherwise, move on to the next pc as normal. */
uint8_t Cpu::CALL_flag_u16(CpuFlags flag, uint8_t value)
{
  uint8_t lsb = bus->read(pc++);
  uint8_t msb = bus->read(pc++);
  uint16_t address = (msb << 8) | lsb;

  if (GetFlag(flag) == value) {
    Push16Bit(pc);
    pc = address;
    return 16;
  } else {
    return 12;
  }
}

uint8_t Cpu::CALL_NZ_u16()  { return CALL_flag_u16(ZERO, 0); }
uint8_t Cpu::CALL_Z_u16()   { return CALL_flag_u16(ZERO, 1); }
uint8_t Cpu::CALL_NC_u16()  { return CALL_flag_u16(CARRY, 0); }
uint8_t Cpu::CALL_C_u16()   { return CALL_flag_u16(CARRY, 1); }

/* CD: CALL u16
 * Push PC+1 to stack and jump to address  */
uint8_t Cpu::CALL_u16() {
  uint8_t lsb = bus->read(pc++);
  uint8_t msb = bus->read(pc++);
  uint16_t address = (msb << 8) | lsb;
  Push16Bit(pc);
  pc = address;
  return 24;
}

/* ========== RESTARTS ========== */
/* C7: Push present address to stack. 
 * Jump to address $0000 + n. ??? */
// THIS IS NOT CORRECT
uint8_t Cpu::RST_00h() {
  // Push 
  Push16Bit(pc);
  pc = 0;
  return 32;
}

uint8_t Cpu::RST_08h() {
  Push16Bit(pc);
  pc = 0x08;
  return 32;
}

uint8_t Cpu::RST_10h() {
  Push16Bit(pc);
  pc = 0x08;
  return 32;
}

uint8_t Cpu::RST_18h() {
  Push16Bit(pc);
  pc = 0x18;
  return 32;
}

uint8_t Cpu::RST_20h() {
  Push16Bit(pc);
  pc = 0x20;
  return 32;
}

uint8_t Cpu::RST_28h() {
  Push16Bit(pc);
  pc = 0x28;
  return 32;
}

uint8_t Cpu::RST_30h() {
  Push16Bit(pc);
  pc = 0x30;
  return 32;
}

uint8_t Cpu::RST_38h() {
  Push16Bit(pc);
  pc = 0x38;
  return 32;
}
