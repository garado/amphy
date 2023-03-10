
/* ░░█ █░█ █▀▄▀█ █▀█ ░░▄▀ █▀▀ ▄▀█ █░░ █░░ ░░▄▀ █▀▄▀█ █ █▀ █▀▀ */ 
/* █▄█ █▄█ █░▀░█ █▀▀ ▄▀░░ █▄▄ █▀█ █▄▄ █▄▄ ▄▀░░ █░▀░█ █ ▄█ █▄▄ */ 

#include "cpu.h"
#include "../defines.h"

/* ========== JUMPS ========== */

/* E9: JP HL
 * Set pc to HL. */
void Cpu::JP_HL() {
  pc = hl();
  cycles_last_taken = 4;
}

/* 18: JR n
 * Add n to current pc. */
void Cpu::JR_i8() {
  int8_t i8 = bus->read(pc++);
  pc += i8;
  cycles_last_taken = 8;
}

/* C3: JP nn
 * Jump to address nn */
void Cpu::JP_u16() {
  uint8_t lsb = bus->read(pc++);
  uint8_t msb = bus->read(pc++);
  pc = (msb << 8) | lsb;
  cycles_last_taken = 16;
}

/* JP_flag_u16
 * Load 16bit immediate operand into pc if flag == value. 
 * Otherwise, move on to the next pc as normal. */
void Cpu::JP_flag_u16(CpuFlags flag, uint8_t value)
{
  uint8_t lsb = bus->read(pc++);
  uint8_t msb = bus->read(pc++);
  uint16_t address = (msb << 8) | lsb;

  if (GetFlag(flag) == value) {
    pc = address;
    cycles_last_taken = 16;
  } else {
    cycles_last_taken = 12;
  }
}

void Cpu::JP_NZ_u16()  { JP_flag_u16(ZERO, 0); }
void Cpu::JP_Z_u16()   { JP_flag_u16(ZERO, 1); }
void Cpu::JP_NC_u16()  { JP_flag_u16(CARRY, 0); }
void Cpu::JP_C_u16()   { JP_flag_u16(CARRY, 1); }

/* JR_flag_i8
 * Add n to current pc if flag == condition.
 * Otherwise go to next address as normal. */
void Cpu::JR_flag_i8(CpuFlags flag, uint8_t val)
{
  int8_t i8 = bus->read(pc++);

  if (GetFlag(flag) == val) {
    pc += i8;
    cycles_last_taken = 12;
  } else {
    cycles_last_taken = 8;
  }
}

void Cpu::JR_NZ_i8()  { JR_flag_i8(ZERO, 0); }
void Cpu::JR_Z_i8()   { JR_flag_i8(ZERO, 1); }
void Cpu::JR_NC_i8()  { JR_flag_i8(CARRY, 0); }
void Cpu::JR_C_i8()   { JR_flag_i8(CARRY, 1); }

/* ========== MISCELLANEOUS ========== */

/* 00: NOP
 * Does nothing. */
void Cpu::NOP() {
  cycles_last_taken = 4;
}

/* 2F: CPL
 * Complement A register. */
void Cpu::CPL() {
  AssignFlag(SUB, 1);
  AssignFlag(HALF_CARRY, 1);
  a = ~a;
  cycles_last_taken = 4;
}

/* 3F: CCF
 * Complement carry flag. */
void Cpu::CCF() {
  uint8_t cy = GetFlag(CARRY);
  AssignFlag(CARRY, !cy);
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 0);
  cycles_last_taken = 4;
}

/* 37: SCF
 * Set carry flag. */
void Cpu::SCF() {
  AssignFlag(CARRY, 1);
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 0);
  cycles_last_taken = 4;
}

/* 07: HALT
 * Power down CPU til interrupt occurs. */
void Cpu::HALT() {
  // !!! INTERRUPT HANDLER HAS NOT BEEN WRITTEN
  // THIS FUNCTION MAY NEED TO BE MODIFIED WHEN THAT'S DONE
  cpuEnabled = false;
  cycles_last_taken = 4;
}

/* 27: DAA 
 * Called immediately after 2 BCD-encoded digits are added.
 * Responsible for adjust accumulator to correct BCD representation. 
 * Sets carry flag if result > 0x99. */
void Cpu::DAA() {
  uint16_t correction = 0; 

  uint8_t HC = GetFlag(HALF_CARRY);
  uint8_t C = GetFlag(CARRY);
  uint8_t N = GetFlag(SUB);

  if (HC || (!N && ((a & 0xF) > 0x9))) {
    correction |= 0x6;
  }
  
  if (C || (!N && (a > 0x99))) {
    correction |= 0x60;
    AssignFlag(CARRY, true);
  }

  a += N ? -correction : correction;

  AssignFlag(HALF_CARRY, 0);
  AssignFlag(ZERO, a == 0);
  cycles_last_taken = 4;
}

/* 10 00:STOP
 * Halt CPU and LCD until button pressed. */
void Cpu::STOP() {
  // THIS FUNCTION IS JANK
  pc += 2;
  cycles_last_taken = 4;
}

/* F3: DI
 * Disables interrupts (IME = 0) but NOT immediately.
 * Interrupts are disabled after instruction after DI is 
 * executed. */
void Cpu::DI() {
  disableInterrupts = 2; // magic number ugh
  cycles_last_taken = 4;
}

/* FB: EI
 * Enables interrupts (IME = 0) but NOT immediately.
 * Interrupts are enabled after the instruction after EI is 
 * executed. */
void Cpu::EI() {
  enableInterrupts = 2; // magic number ugh
  cycles_last_taken = 4;
}

/* ========== RETURNS ========== */
/* C0: RET_NZ
 * If Z flag is 0, pop address off stack and jump to it*/
 void Cpu::RET_NZ() {
  if (GetFlag(ZERO) == 0) {
    uint8_t lsb = bus->read(sp++);
    uint8_t msb = bus->read(sp++);
    pc = (msb << 8) | lsb;
    cycles_last_taken = 20;
  } else {
    cycles_last_taken = 8;
  }
 }

 /* C8: RET_Z
 * If Z flag is 1, pop address off stack and jump to it*/
 void Cpu::RET_Z() {
  if (GetFlag(ZERO) == 1) {
    uint8_t lsb = bus->read(sp++);
    uint8_t msb = bus->read(sp++);
    pc = (msb << 8) | lsb;
    cycles_last_taken = 20;
  } else {
    cycles_last_taken = 8;
  }
 }

/* C9: RET
 * Returns from a subroutine call. Pops address off stack and
 * jumps to address. */
void Cpu::RET() {
  pc = Pop16Bit();
  cycles_last_taken = 16;
}

/* D9: RETI
 * cycles_last_taken = from an interrupt. Sets IME flag back to its
 * pre-interrupt status. */
void Cpu::RETI() {
  uint8_t lsb = bus->read(sp++);
  uint8_t msb = bus->read(sp++);
  pc = (msb << 8) | lsb;
  ime = ime_prev;
  cycles_last_taken = 16;
}

/* D0: RET_NC
 * If Carry flag is 0, pop address off stack and jump to it*/
 void Cpu::RET_NC() {
  if (GetFlag(CARRY) == 0) {
    uint8_t lsb = bus->read(sp++);
    uint8_t msb = bus->read(sp++);
    pc = (msb << 8) | lsb;
    cycles_last_taken = 20;
  } else {
    cycles_last_taken = 8;
  }
 }

 /* D8: RET_C
 * If Z flag is 1, pop address off stack and jump to it*/
 void Cpu::RET_C() {
  if (GetFlag(CARRY) == 1) {
    uint8_t lsb = bus->read(sp++);
    uint8_t msb = bus->read(sp++);
    pc = (msb << 8) | lsb;
    cycles_last_taken = 20;
  } else {
    cycles_last_taken = 8;
  }
 }

/* ========== CALLS ========== */
/* CALL_flag_u16
 * Load 16bit immediate operand into pc if flag == value. 
 * Otherwise, move on to the next pc as normal. */
void Cpu::CALL_flag_u16(CpuFlags flag, uint8_t value)
{
  uint8_t lsb = bus->read(pc++);
  uint8_t msb = bus->read(pc++);
  uint16_t address = (msb << 8) | lsb;

  if (GetFlag(flag) == value) {
    Push16Bit(pc);
    pc = address;
    cycles_last_taken = 16;
  } else {
    cycles_last_taken = 12;
  }
}

void Cpu::CALL_NZ_u16()  { CALL_flag_u16(ZERO, 0); }
void Cpu::CALL_Z_u16()   { CALL_flag_u16(ZERO, 1); }
void Cpu::CALL_NC_u16()  { CALL_flag_u16(CARRY, 0); }
void Cpu::CALL_C_u16()   { CALL_flag_u16(CARRY, 1); }

/* CD: CALL u16
 * Push PC+1 to stack and jump to address  */
void Cpu::CALL_u16() {
  uint8_t lsb = bus->read(pc++);
  uint8_t msb = bus->read(pc++);
  uint16_t address = (msb << 8) | lsb;
  Push16Bit(pc);
  pc = address;
  cycles_last_taken = 24;
}

/* ========== RESTARTS ========== */
/* C7: Push present address to stack. 
 * Jump to address $0000 + n. ??? */
// THIS IS NOT CORRECT
void Cpu::RST_00h() {
  // Push 
  Push16Bit(pc);
  pc = 0;
  cycles_last_taken = 32;
}

void Cpu::RST_08h() {
  Push16Bit(pc);
  pc = 0x08;
  cycles_last_taken = 32;
}

void Cpu::RST_10h() {
  Push16Bit(pc);
  pc = 0x08;
  cycles_last_taken = 32;
}

void Cpu::RST_18h() {
  Push16Bit(pc);
  pc = 0x18;
  cycles_last_taken = 32;
}

void Cpu::RST_20h() {
  Push16Bit(pc);
  pc = 0x20;
  cycles_last_taken = 32;
}

void Cpu::RST_28h() {
  Push16Bit(pc);
  pc = 0x28;
  cycles_last_taken = 32;
}

void Cpu::RST_30h() {
  Push16Bit(pc);
  pc = 0x30;
  cycles_last_taken = 32;
}

void Cpu::RST_38h() {
  Push16Bit(pc);
  pc = 0x38;
  cycles_last_taken = 32;
}
