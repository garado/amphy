
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
  int8_t i8 = bus->read(++pc);
  pc += i8;
  return 8;
}

/*  C3: JP nn
 *  Jump to address nn */
uint8_t Cpu::JP_u16() {
  uint8_t lsb = bus->read(++pc);
  uint8_t msb = bus->read(++pc);
  pc = (msb << 8) | lsb;
  return 12;
}

/* JP cc, nn
 * Jump to address nn if flag == condition */
uint8_t Cpu::JP_NZ_u16() {
  uint8_t lsb = bus->read(++pc);
  uint8_t msb = bus->read(++pc);
  uint8_t address = (msb << 8) | lsb;
  ++pc;
  bool flag = GetFlag(ZERO);
  if (flag == 0) {
    pc = address;
  }
  return 12;
}

uint8_t Cpu::JP_Z_u16() {
  uint8_t lsb = bus->read(++pc);
  uint8_t msb = bus->read(++pc);
  uint8_t address = (msb << 8) | lsb;
  ++pc;
  bool flag = GetFlag(ZERO);
  if (flag == 1) {
    pc = address;
  }
  return 12;
}

uint8_t Cpu::JP_NC_u16() {
  uint8_t lsb = bus->read(++pc);
  uint8_t msb = bus->read(++pc);
  uint8_t address = (msb << 8) | lsb;
  ++pc;
  bool flag = GetFlag(CARRY);
  if (flag == 0) {
    pc = address;
  }
  return 12;
}

uint8_t Cpu::JP_C_u16() {
  uint8_t lsb = bus->read(++pc);
  uint8_t msb = bus->read(++pc);
  uint8_t address = (msb << 8) | lsb;
  ++pc;
  bool flag = GetFlag(CARRY);
  if (flag == 1) {
    pc = address;
  }
  return 12;
}

/* JR cc, n
 * Add n to current address if flag == condition */
uint8_t Cpu::JR_NZ_i8() {
  int8_t u8 = bus->read(++pc);
  ++pc;
  bool flag = GetFlag(ZERO);
  if (flag == 0) {
    pc += u8;
  }
  return 8;
}

uint8_t Cpu::JR_Z_i8() {
  int8_t u8 = bus->read(++pc);
  ++pc;
  bool flag = GetFlag(ZERO);
  if (flag == 1) {
    pc += u8;
  }
  return 8;
}

uint8_t Cpu::JR_NC_i8() {
  int8_t u8 = bus->read(++pc);
  ++pc;
  bool flag = GetFlag(CARRY);
  if (flag == 0) {
    pc += u8;
  }
  return 8;
}

uint8_t Cpu::JR_C_i8() {
  int8_t u8 = bus->read(++pc);
  ++pc;
  bool flag = GetFlag(CARRY);
  if (flag == 1) {
    pc += u8;
  }
  return 8;
}

/* ========== MISCELLANEOUS ========== */

/* 00: NOP
 * Does nothing. */
uint8_t Cpu::NOP() {
  ++pc;
  return 4;
}

/* 2F: CPL
 * Complement A register. */
uint8_t Cpu::CPL() {
  AssignFlag(ZERO, 0);
  AssignFlag(HALF_CARRY, 0);
  a = ~a;
  ++pc;
  return 4;
}

/* 3F: CCF
 * Complement carry flag. */
uint8_t Cpu::CCF() {
  uint8_t cy = GetFlag(CARRY);
  AssignFlag(CARRY, ~cy);
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 0);
  ++pc;
  return 4;
}

/* 37: SCF
 * Set carry flag. */
uint8_t Cpu::SCF() {
  AssignFlag(CARRY, 1);
  AssignFlag(SUB, 0);
  AssignFlag(HALF_CARRY, 0);
  ++pc;
  return 4;
}

/* 07: HALT
 * Power down CPU til interrupt occurs. */
uint8_t Cpu::HALT() {
  // !!! INTERRUPT HANDLER HAS NOT BEEN WRITTEN
  // THIS FUNCTION MAY NEED TO BE MODIFIED WHEN THAT'S DONE
  cpuEnabled = false;
  ++pc;
  return 4;
}

/* 27: DAA
 * Tbh I do not understand wtf this does */
 uint8_t Cpu::DAA() {
  ++pc;
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
  ++pc;
  return 4;
}

/* FB: EI
 * Enables interrupts (IME = 0) but NOT immediately.
 * Interrupts are enabled after instruction after EI is 
 * executed. */
uint8_t Cpu::EI() {
  enableInterrupts = 2; // magic number ugh
  ++pc;
  return 4;
}

/* ========== RETURNS ========== */
/* C0: RET_NZ
 * If Z flag is 0, pop address off stack and jump to it*/
 uint8_t Cpu::RET_NZ() {
  bool flag = GetFlag(ZERO);
  if (flag == 0) {
    uint8_t lsb = bus->read(sp++);
    uint8_t msb = bus->read(sp++);
    pc = (msb << 8) | lsb;
    return 20;
  } else {
    ++pc;
    return 20;
  }
 }

 /* C8: RET_Z
 * If Z flag is 1, pop address off stack and jump to it*/
 uint8_t Cpu::RET_Z() {
  bool flag = GetFlag(ZERO);
  if (flag == 1) {
    uint8_t lsb = bus->read(sp++);
    uint8_t msb = bus->read(sp++);
    pc = (msb << 8) | lsb;
    return 20;
  } else {
    ++pc;
    return 20;
  }
 }

/* C9: RET
 * Pop address off stack and jump to it*/
uint8_t Cpu::RET() {
  uint8_t lsb = bus->read(sp++);
  uint8_t msb = bus->read(sp++);
  pc = (msb << 8) | lsb;
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
  bool flag = GetFlag(CARRY);
  if (flag == 0) {
    uint8_t lsb = bus->read(sp++);
    uint8_t msb = bus->read(sp++);
    pc = (msb << 8) | lsb;
    return 20;
  } else {
    ++pc;
    return 20;
  }
 }

 /* D8: RET_C
 * If Z flag is 1, pop address off stack and jump to it*/
 uint8_t Cpu::RET_C() {
  bool flag = GetFlag(CARRY);
  if (flag == 1) {
    uint8_t lsb = bus->read(sp++);
    uint8_t msb = bus->read(sp++);
    pc = (msb << 8) | lsb;
    return 20;
  } else {
    ++pc;
    return 20;
  }
 }

/* ========== CALLS ========== */
/* C4: CALL NZ, u16
 * If Z == 0, push PC to stack and call  */
uint8_t Cpu::CALL_NZ_u16() {
  bool flag = GetFlag(ZERO);
  if (flag == 0) {
    uint8_t addr = pc + 1;    // push
    uint8_t msb = addr >> 8;
    uint8_t lsb = addr & 0xFF;
    bus->write(--sp, msb);
    bus->write(--sp, lsb);

    uint16_t u16 = bus->read(++pc); // new pc
    pc = u16;
    return 24;
  } else {
    pc += 2;
    return 12;
  }
}

/* CC: CALL Z, u16
 * If Z == 1, push PC to stack and call  */
uint8_t Cpu::CALL_Z_u16() {
  bool flag = GetFlag(ZERO);
  if (flag == 1) {
    uint8_t addr = pc + 1;    // push
    uint8_t msb = addr >> 8;
    uint8_t lsb = addr & 0xFF;
    bus->write(--sp, msb);
    bus->write(--sp, lsb);

    uint16_t u16 = bus->read(++pc); // new pc
    pc = u16;
    return 24;
  } else {
    pc += 2;
    return 12;
  }
}

/* CD: CALL u16
 * Push PC+1 to stack and jump to address  */
uint8_t Cpu::CALL_u16() {
  uint8_t addr = pc + 1;    // push
  uint8_t msb = addr >> 8;
  uint8_t lsb = addr & 0xFF;
  bus->write(--sp, msb);
  bus->write(--sp, lsb);

  uint16_t u16 = bus->read(++pc); // new pc
  pc = u16;
  return 24;
}

/* D4: CALL NC, u16
 * If Carry == 0, push PC to stack and call  */
uint8_t Cpu::CALL_NC_u16() {
  bool flag = GetFlag(CARRY);
  if (flag == 0) {
    uint8_t addr = pc + 1;    // push
    uint8_t msb = addr >> 8;
    uint8_t lsb = addr & 0xFF;
    bus->write(--sp, msb);
    bus->write(--sp, lsb);

    uint16_t u16 = bus->read(++pc); // new pc
    pc = u16;
    return 24;
  } else {
    pc += 2;
    return 12;
  }
}

/* DC: CALL C, u16
 * If Carry == 1, push PC to stack and call  */
uint8_t Cpu::CALL_C_u16() {
  bool flag = GetFlag(CARRY);
  if (flag == 1) {
    uint8_t addr = pc + 1;    // push
    uint8_t msb = addr >> 8;
    uint8_t lsb = addr & 0xFF;
    bus->write(--sp, msb);
    bus->write(--sp, lsb);

    uint16_t u16 = bus->read(++pc); // new pc
    pc = u16;
    return 24;
  } else {
    pc += 2;
    return 12;
  }
}

/* ========== RESTARTS ========== */
/* Push present address to stack.
 * Jump to address $0000 + n. */
uint8_t Cpu::RST_00h() {
  // Push 
  uint8_t msb = pc >> 8;
  uint8_t lsb = pc & 0xFF;
  bus->write(--sp, msb);
  bus->write(--sp, lsb);
  pc = 0;
  return 32;
}

uint8_t Cpu::RST_08h() {
  // Push 
  uint8_t msb = pc >> 8;
  uint8_t lsb = pc & 0xFF;
  bus->write(--sp, msb);
  bus->write(--sp, lsb);
  pc = 0x08;
  return 32;
}

uint8_t Cpu::RST_10h() {
  // Push 
  uint8_t msb = pc >> 8;
  uint8_t lsb = pc & 0xFF;
  bus->write(--sp, msb);
  bus->write(--sp, lsb);
  pc = 0x08;
  return 32;
}

uint8_t Cpu::RST_18h() {
  // Push 
  uint8_t msb = pc >> 8;
  uint8_t lsb = pc & 0xFF;
  bus->write(--sp, msb);
  bus->write(--sp, lsb);
  pc = 0x18;
  return 32;
}

uint8_t Cpu::RST_20h() {
  // Push 
  uint8_t msb = pc >> 8;
  uint8_t lsb = pc & 0xFF;
  bus->write(--sp, msb);
  bus->write(--sp, lsb);
  pc = 0x20;
  return 32;
}

uint8_t Cpu::RST_28h() {
  // Push 
  uint8_t msb = pc >> 8;
  uint8_t lsb = pc & 0xFF;
  bus->write(--sp, msb);
  bus->write(--sp, lsb);
  pc = 0x28;
  return 32;
}

uint8_t Cpu::RST_30h() {
  // Push 
  uint8_t msb = pc >> 8;
  uint8_t lsb = pc & 0xFF;
  bus->write(--sp, msb);
  bus->write(--sp, lsb);
  pc = 0x30;
  return 32;
}

uint8_t Cpu::RST_38h() {
  // Push 
  uint8_t msb = pc >> 8;
  uint8_t lsb = pc & 0xFF;
  bus->write(--sp, msb);
  bus->write(--sp, lsb);
  pc = 0x38;
  return 32;
}
