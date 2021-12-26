
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
    bool flag = getFlag(ZERO);
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
    bool flag = getFlag(ZERO);
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
    bool flag = getFlag(CARRY);
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
    bool flag = getFlag(CARRY);
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
    bool flag = getFlag(ZERO);
    if (flag == 0) {
        pc += u8;
    }
    return 8;
}

uint8_t Cpu::JR_Z_i8() {
    int8_t u8 = bus->read(++pc);
    ++pc;
    bool flag = getFlag(ZERO);
    if (flag == 1) {
        pc += u8;
    }
    return 8;
}

uint8_t Cpu::JR_NC_i8() {
    int8_t u8 = bus->read(++pc);
    ++pc;
    bool flag = getFlag(CARRY);
    if (flag == 0) {
        pc += u8;
    }
    return 8;
}

uint8_t Cpu::JR_C_i8() {
    int8_t u8 = bus->read(++pc);
    ++pc;
    bool flag = getFlag(CARRY);
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
    assignFlag(ZERO, 0);
    assignFlag(HALF_CARRY, 0);
    a = ~a;
    ++pc;
    return 4;
}

/* 3F: CCF
 * Complement carry flag. */
uint8_t Cpu::CCF() {
    uint8_t cy = getFlag(CARRY);
    assignFlag(CARRY, ~cy);
    assignFlag(SUB, 0);
    assignFlag(HALF_CARRY, 0);
    ++pc;
    return 4;
}

/* 37: SCF
 * Set carry flag. */
uint8_t Cpu::SCF() {
    assignFlag(CARRY, 1);
    assignFlag(SUB, 0);
    assignFlag(HALF_CARRY, 0);
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

uint8_t Cpu::RET_Z() {

}

uint8_t Cpu::RET_C() {

}

uint8_t Cpu::RET_NZ() {

}

uint8_t Cpu::RET_NC() {

}

/* ========== CALLS ========== */
uint8_t Cpu::CALL_Z_u16() {

}

uint8_t Cpu::CALL_NZ_u16() {

}

uint8_t Cpu::CALL_u16() {

}

uint8_t Cpu::CALL_C_u16() {

}

uint8_t Cpu::CALL_NC_u16() {

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