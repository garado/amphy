
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "cpu.h"

/* Cpu::execute
 * Handles execution of all opcodes */
bool Cpu::execute() {
    uint8_t opcode = bus->read(pc);

    switch (opcode) {
        case 0x01: LD_R1R2_nn(&b, &c);  break;
        case 0x02: LD_BC_A();       break;

        case 0x04: INC_R1(&b);      break;
        case 0x05: DEC_R1(&b);      break;
        case 0x06: LD_R1_n(&b);     break;

        case 0x08: LD_nn_SP();      break;

        case 0x0A: LD_A_BC();       break;  // ???

        case 0x0C: INC_R1(&c);      break;
        case 0x0D: DEC_R1(&c);      break;
        case 0x0E: LD_R1_n(&c);     break;
        
        case 0x11: LD_R1R2_nn(&d, &e);  break;
        case 0x12: LD_DE_A();       break;

        case 0x14: INC_R1(&d);      break;
        case 0x15: DEC_R1(&d);      break;
        case 0x16: LD_R1_n(&d);     break;
        
        case 0x18: JR_n();          break;

        case 0x1A: LD_A_DE();       break;

        case 0x1C: INC_R1(&e);      break;
        case 0x1D: DEC_R1(&e);      break;
        case 0x1E: LD_R1_n(&e);     break;

        case 0x20: JR_cc_n(ZERO, 0);    break;
        case 0x21: LD_R1R2_nn(&h, &l);  break;
        case 0x22: LDI_A_HL();      break;
        
        case 0x24: INC_R1(&h);      break;
        case 0x25: DEC_R1(&h);      break;
        case 0x26: LD_R1_n(&h);     break;

        case 0x28: JR_cc_n(ZERO, 1);    break;
        
        case 0x2A: LDI_A_HL();      break;

        case 0x2C: INC_R1(&l);      break;
        case 0x2D: DEC_R1(&l);      break;
        case 0x2E: LD_R1_n(&l);     break;

        case 0x30: JR_cc_n(CARRY, 0);    break;
        case 0x31: LD_SP_nn();      break;
        case 0x32: LDD_HL_A();      break;

        case 0x34: INC_HL();        break;
        case 0x35: INC_HL();        break;
        case 0x36: LD_HL_n();       break;

        case 0x38: JR_cc_n(CARRY, 1);    break;

        case 0x3A: LDD_A_HL();      break;

        case 0x3C: INC_R1(&a);      break;

        case 0x3D: DEC_R1(&a);      break;
        case 0x3E: LD_A_n();        break;
        
        case 0x40: LD_R1_R2(&b, &b);    break;
        case 0x41: LD_R1_R2(&b, &c);    break;
        case 0x42: LD_R1_R2(&b, &d);    break;
        case 0x43: LD_R1_R2(&b, &e);    break;
        case 0x44: LD_R1_R2(&b, &h);    break;
        case 0x45: LD_R1_R2(&b, &l);    break;
        case 0x46: LD_R1_HL(&b);        break;

        case 0x48: LD_R1_R2(&c, &b);    break;
        case 0x49: LD_R1_R2(&c, &c);    break;
        case 0x4A: LD_R1_R2(&c, &d);    break;
        case 0x4B: LD_R1_R2(&c, &e);    break;
        case 0x4C: LD_R1_R2(&c, &h);    break;
        case 0x4D: LD_R1_R2(&c, &l);    break;
        case 0x4E: LD_R1_HL(&c);        break;

        case 0x50: LD_R1_R2(&d, &b);    break;
        case 0x51: LD_R1_R2(&d, &c);    break;
        case 0x52: LD_R1_R2(&d, &d);    break;
        case 0x53: LD_R1_R2(&d, &e);    break;
        case 0x54: LD_R1_R2(&d, &h);    break;
        case 0x55: LD_R1_R2(&d, &l);    break;
        case 0x56: LD_R1_HL(&d);        break;

        case 0x58: LD_R1_R2(&e, &b);    break;
        case 0x59: LD_R1_R2(&e, &c);    break;
        case 0x5A: LD_R1_R2(&e, &d);    break;
        case 0x5B: LD_R1_R2(&e, &e);    break;
        case 0x5C: LD_R1_R2(&e, &h);    break;
        case 0x5D: LD_R1_R2(&e, &l);    break;
        case 0x5E: LD_R1_HL(&e);        break;

        case 0x60: LD_R1_R2(&h, &b);    break;
        case 0x61: LD_R1_R2(&h, &c);    break;
        case 0x62: LD_R1_R2(&h, &d);    break;
        case 0x63: LD_R1_R2(&h, &e);    break;
        case 0x64: LD_R1_R2(&h, &h);    break;
        case 0x65: LD_R1_R2(&h, &l);    break;
        case 0x66: LD_R1_HL(&h);        break;

        case 0x68: LD_R1_R2(&l, &b);    break;
        case 0x69: LD_R1_R2(&l, &c);    break;
        case 0x6A: LD_R1_R2(&l, &d);    break;
        case 0x6B: LD_R1_R2(&l, &e);    break;
        case 0x6C: LD_R1_R2(&l, &h);    break;
        case 0x6D: LD_R1_R2(&l, &l);    break;
        case 0x6E: LD_R1_HL(&l);        break;

        case 0x70: LD_HL_R1(&b);        break;
        case 0x71: LD_HL_R1(&c);        break;
        case 0x72: LD_HL_R1(&d);        break;
        case 0x73: LD_HL_R1(&e);        break;
        case 0x74: LD_HL_R1(&h);        break;
        case 0x75: LD_HL_R1(&l);        break;

        case 0x77: LD_HL_A();           break;
        case 0x78: LD_R1_R2(&a, &b);    break;
        case 0x79: LD_R1_R2(&a, &c);    break;
        case 0x7A: LD_R1_R2(&a, &d);    break;
        case 0x7B: LD_R1_R2(&a, &e);    break;
        case 0x7C: LD_R1_R2(&a, &h);    break;
        case 0x7D: LD_R1_R2(&a, &l);    break;
        case 0x7E: LD_R1_HL(&a);        break;
        case 0x7F: LD_R1_R2(&a, &a);    break;

        case 0x80: ADD_A_R1(&b);        break;
        case 0x81: ADD_A_R1(&c);        break;
        case 0x82: ADD_A_R1(&d);        break;
        case 0x83: ADD_A_R1(&e);        break;
        case 0x84: ADD_A_R1(&h);        break;
        case 0x85: ADD_A_R1(&l);        break;
        case 0x86: ADD_A_HL();          break;

        case 0x87: ADD_A_R1(&a);        break;
        case 0x88: ADC_A_R1(&b);        break;
        case 0x89: ADC_A_R1(&c);        break;
        case 0x8A: ADC_A_R1(&d);        break;
        case 0x8B: ADC_A_R1(&e);        break;
        case 0x8C: ADC_A_R1(&h);        break;
        case 0x8D: ADC_A_R1(&l);        break;
        case 0x8E: ADC_A_HL();          break;
        case 0x8F: ADC_A_R1(&a);        break;
        case 0x90: SUB_R1(&b);          break;
        case 0x91: SUB_R1(&c);          break;
        case 0x92: SUB_R1(&d);          break;
        case 0x93: SUB_R1(&e);          break;
        case 0x94: SUB_R1(&h);          break;
        case 0x95: SUB_R1(&l);          break;
        case 0x96: SUB_HL();            break;
        case 0x97: SUB_R1(&a);          break;
        case 0x98: SBC_A_R1(&b);        break;
        case 0x99: SBC_A_R1(&c);        break;
        case 0x9A: SBC_A_R1(&d);        break;
        case 0x9B: SBC_A_R1(&e);        break;
        case 0x9C: SBC_A_R1(&h);        break;
        case 0x9D: SBC_A_R1(&l);        break;

        case 0x9F: SBC_A_R1(&a);        break;

        case 0xA0: AND_R1(&b);          break;
        case 0xA1: AND_R1(&c);          break;
        case 0xA2: AND_R1(&d);          break;
        case 0xA3: AND_R1(&e);          break;
        case 0xA4: AND_R1(&h);          break;
        case 0xA5: AND_R1(&l);          break;
        case 0xA6: AND_HL();            break;
        case 0xA7: AND_R1(&a);          break;
        case 0xA8: XOR_R1(&b);          break;
        case 0xA9: XOR_R1(&c);          break;
        case 0xAA: XOR_R1(&d);          break;
        case 0xAB: XOR_R1(&e);          break;
        case 0xAC: XOR_R1(&h);          break;
        case 0xAD: XOR_R1(&l);          break;
        case 0xAE: XOR_HL();            break;
        case 0xAF: XOR_R1(&a);          break;

        case 0xB0: OR_R1(&b);           break;
        case 0xB1: OR_R1(&c);           break;
        case 0xB2: OR_R1(&d);           break;
        case 0xB3: OR_R1(&e);           break;
        case 0xB4: OR_R1(&h);           break;
        case 0xB5: OR_R1(&l);           break;
        case 0xB6: OR_HL();             break;
        case 0xB7: OR_R1(&a);           break;
        case 0xB8: CP_R1(&b);           break;
        case 0xB9: CP_R1(&c);           break;
        case 0xBA: CP_R1(&d);           break;
        case 0xBB: CP_R1(&e);           break;
        case 0xBC: CP_R1(&h);           break;
        case 0xBD: CP_R1(&l);           break;
        case 0xBE: CP_HL();             break;
        case 0xBF: CP_R1(&a);           break;

        case 0xC1: POP_R1R2(&b, &c);    break;
        case 0xC2: JP_cc_nn(ZERO, 0);   break;
        case 0xC3: JP_nn();             break;

        case 0xC5: PUSH_R1R2(&b, &c);   break;
        case 0xC6: ADD_A_n();           break;

        case 0xCA: JP_cc_nn(ZERO, 1);   break;

        case 0xCE: ADC_A_n();           break;

        case 0xD1: POP_R1R2(&d, &e);    break;
        case 0xD2: JP_cc_nn(CARRY, 0);  break;

        case 0xD5: PUSH_R1R2(&d, &e);   break;
        case 0xD6: SUB_n();             break;

        case 0xDA: JP_cc_nn(CARRY, 1);  break;

        case 0xE0: LDH_n_A();           break;
        case 0xE1: POP_R1R2(&h, &l);    break;
        case 0xE2: LD_C_A();            break;

        case 0xE5: PUSH_R1R2(&h, &l);   break;
        case 0xE6: AND_n();             break;

        case 0xE9: JP_HL();             break;
        case 0xEA: LD_nn_A();           break;

        case 0xEE: XOR_n();             break;

        case 0xF0: LDH_A_n();           break;
        case 0xF1: POP_R1R2(&a, &f);    break;
        case 0xF2: LD_A_C();            break;

        case 0xF5: PUSH_R1R2(&a, &f);   break;

        case 0xF9: LD_SP_HL();          break;
        case 0xFA: LD_A_nn();           break;

        case 0xFE: CP_n();              break;

        default:
            unknown(opcode);
            return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
};

// ================= OPCODES ================= //

// ========== 8-BIT LOADS ==========
/* LD R1, n
 * Load 8-bit immed operand n into reg */
void Cpu::LD_R1_n(uint8_t* reg) {
    uint8_t n = bus->read(++pc);
    *reg = n;
    ++pc;
    cycleCount += 8;
}

/*  LD R1, R2
 *  Put value R2 into R1. */
void Cpu::LD_R1_R2(uint8_t* reg1, uint8_t* reg2) {
    *reg1 = *reg2;
    ++pc;
    cycleCount += 4;
}

/*  LD R1, (HL)
 *  Put value in mem addres in HL into R1. */
void Cpu::LD_R1_HL(uint8_t* reg1) {
    uint16_t address = (h << 8) | l;
    *reg1 = bus->read(address);
    ++pc;
    cycleCount += 8;
}

/*  LD (HL), R1
 *  Put contents of reg1 into mem location specified by HL. */
void Cpu::LD_HL_R1(uint8_t* reg1) {
    uint16_t address = (h << 8) | l;
    bus->write(address, *reg1);
    ++pc;
    cycleCount += 8;
}

/*  LD (HL), n
 *  Put contents of 8bit immed n into mem location specified by HL. */
void Cpu::LD_HL_n() {
    uint16_t address = (h << 8) | l;
    uint8_t val = bus->read(++pc);
    bus->write(address, val);
    ++pc;
    cycleCount += 12;
}

/*  0A: LD A, (BC)
 *  Put value in mem address in BC into A. */
void Cpu::LD_A_BC() {
    uint16_t address = (b << 8) | c;
    uint8_t val = bus->read(address);
    a = address;
    ++pc;
    cycleCount += 8;
}

/*  1A: LD A, (DE)
 *  Put value in mem address in BC into A. */
void Cpu::LD_A_DE() {
    uint16_t address = (d << 8) | e;
    uint8_t val = bus->read(address);
    a = address;
    ++pc;
    cycleCount += 8;
}

/*  FA: LD A, (nn)
 *  Load value in mem address specified by 16-bit immediate nn into A. */
void Cpu::LD_A_nn() {
    uint8_t lsb = bus->read(++pc);
    uint8_t msb = bus->read(++pc);
    uint16_t address = (msb << 8) | lsb;
    a = bus->read(address);
    ++pc;
    cycleCount += 16;
}

/*  3E: LD A, n
 *  Load 8bit immed value into A. */
void Cpu::LD_A_n() {
    uint8_t val = bus->read(++pc);
    a = val;
    ++pc;
    cycleCount += 8;
}

/*  02: LD (BC), A
 *  Store contents of A into mem location specified by BC. */
void Cpu::LD_BC_A() {
    uint16_t address = (b << 8) | c;
    bus->write(address, a);
    ++pc;
    cycleCount += 8;
}

/*  12: LD (DE), A
 *  Store contents of A into mem location specified by DE. */
void Cpu::LD_DE_A() {
    uint16_t address = (d << 8) | e;
    bus->write(address, a);
    ++pc;
    cycleCount += 8;
}

/*  77: LD (HL), A
 *  Store contents of A in mem location specified by HL. */
void Cpu::LD_HL_A() {
    uint16_t address = (h << 8) | l;
    bus->write(address, a);
    ++pc;
    cycleCount += 8;
}

/*  EA: LD (nn), A
 *  Store contents of register A in reg or internal RAM specified by nn */
void Cpu::LD_nn_A() {
    uint8_t lsb = bus->read(++pc);
    uint8_t msb = bus->read(++pc);
    uint16_t address = (msb << 8) | lsb;
    bus->write(address, a);
    ++pc;
    cycleCount += 16;
}

/*  F2: LD A, (C)
 *  Put value at address $FF00 + RegC into A. */
void Cpu::LD_A_C() {
    uint16_t address = 0xFF00 | c;
    a = bus->read(address);
    ++pc;
    cycleCount += 8;
}

/*  E2: LD (C), A
 *  Put A in address $FF00 + RegC. */
void Cpu::LD_C_A() {
    uint16_t address = 0xFF00 | c;
    bus->write(address, a);
    ++pc;
    cycleCount += 8;
}

/*  3A: LDD A, (HL)
 *  Put val at address HL into A. Decrement HL. */
void Cpu::LDD_A_HL() {
    uint16_t address = (h << 8) | l;
    a = bus->read(address);

    // Decrement HL
    address--;
    h = address >> 8;
    l = (address & 0x00FF);
    ++pc;
    cycleCount += 8;
}

/*  32: LDD (HL), A
 *  Put A into address HL. Decrement HL. */
void Cpu::LDD_HL_A() {
    uint16_t address = (h << 8) | l;
    bus->write(address, a);

    // Decrement HL
    address--;
    h = address >> 8;
    l = (address & 0x00FF);
    ++pc;
    cycleCount += 8;
}

/*  2A: LDI A, (HL)
 *  Put val at address HL into A. Increment HL. */
void Cpu::LDI_A_HL() {
    uint16_t address = (h << 8) | l;
    a = bus->read(address);

    // Increment HL
    address++;
    h = address >> 8;
    l = (address & 0x00FF);
    ++pc;
    cycleCount += 8;
}

/*  22: LDI (HL), A
 *  Put A into address HL. Decrement HL. */
void Cpu::LDI_HL_A() {
    uint16_t address = (h << 8) | l;
    bus->write(address, a);

    // Increment HL
    address++;
    h = address >> 8;
    l = (address & 0x00FF);
    ++pc;
    cycleCount += 8;
}

/*  E0: LDH (n), A
 *  Put A into address $FF00 + n; */
void Cpu::LDH_n_A() {
    uint8_t n = bus->read(++pc);
    uint16_t address = 0xFF00 + n;
    bus->write(address, a);
    ++pc;
    cycleCount += 12;
}

/*  F0: LDH (n), A
 *  Put value at address $FF00 + n into A. */
void Cpu::LDH_A_n() {
    uint8_t n = bus->read(++pc);
    uint16_t address = 0xFF00 + n;
    a = bus->read(address);
    ++pc;
    cycleCount += 12;
}

// ========== 16-BIT LOADS ==========
/*  Opcodes: 01 11 21
 *  LD R1R2, nn
 *  Put immed val nn into register pair R1R2 (eg. bc, de, hl) */
void Cpu::LD_R1R2_nn(uint8_t* R1, uint8_t* R2) {
    *R2 = bus->read(++pc); // LSB of nn
    *R1 = bus->read(++pc); // MSB of nn
    ++pc;
    cycleCount += 12;
}

/* 31: LD SP, nn
 * Put immed val nn into SP */
void Cpu::LD_SP_nn() {
    uint8_t lsb = bus->read(++pc);
    uint8_t msb = bus->read(++pc);
    sp = (msb << 8) | lsb;
    ++pc;
    cycleCount += 12;
}

/*  F9: LD SP, HL
 *  Put HL into SP. */
void Cpu::LD_SP_HL() {
    sp = (h << 8) | l;
    ++pc;
    cycleCount += 8;
}

/*  F8: LDHL SP, n
 *  Put SP + n effective address into HL.
 *  Note: n is signed! */
void Cpu::LDHL_SP_n() {
    int8_t n = bus->read(++pc);
    uint16_t newHL = sp + n;
    h = (newHL & 0xFF00) >> 8;
    l = newHL & 0xFF;
    setAddFlags(sp, n);
    resetFlag(ZERO);
    ++pc;
    cycleCount += 12;
}

/*  08: LD (nn), SP
 *  Put SP at address nn.
 *  LSB of SP goes to pc+1, and MSB goes to pc+2. */
void Cpu::LD_nn_SP() {
    uint8_t msb = sp >> 8;
    uint8_t lsb = sp & 0xFF;
    bus->write(++pc, lsb);
    bus->write(++pc, msb);
    ++pc;
    cycleCount += 20;
}

/*  Opcodes: F5 C5 D5 E5
 *  Push register pair R1R2 onto stack. Decrement SP twice. */
void Cpu::PUSH_R1R2(uint8_t* R1, uint8_t* R2) {
    // !!! check for overflow later.
    // CANNOT for the life of me find the stack range.
    bus->write(--sp, *R1);
    bus->write(--sp, *R2);
    ++pc;
    cycleCount += 16;
}

/*  Opcodes: F1 C1 D1 E1
 *  Pop SP to register pair R1R2. Increment SP twice. */
void Cpu::POP_R1R2(uint8_t* R1, uint8_t* R2) {
    // check for underflow later
    *R2 = bus->read(sp++);
    *R1 = bus->read(sp++);
    ++pc;
    cycleCount += 16;
}

// ========== 8-BIT ALU ==========
/*  ADD A, R1
 *  Add R1 to A. */
void Cpu::ADD_A_R1(uint8_t* R1) {
    setAddFlags(a, *R1);
    a += *R1;
    ++pc;
    cycleCount += 4;
}

/*  86: ADD A, (HL)
 *  A = A + (HL) */
void Cpu::ADD_A_HL() {
    uint16_t address = (h << 8) | l;
    uint8_t val = bus->read(address);
    setAddFlags(a, val);
    a += val;
    ++pc;
    cycleCount += 8;
}

/*  C6: ADD A, n
 *  A = A + 8bit n */
void Cpu::ADD_A_n() {
    uint8_t val = bus->read(++pc);
    setAddFlags(a, val);
    a += val;
    ++pc;
    cycleCount += 8;
}

/*  ADC A, R1
 *  A = A + (R1 + carry flag) */
void Cpu::ADC_A_R1(uint8_t* R1) {
    uint8_t val = *R1 + getFlag(CARRY);
    setAddFlags(a, val);
    a += val;
    ++pc;
    cycleCount += 4;
}

/*  8E: ADC A, (HL)
 *  A = A + ((HL) + carry) */
void Cpu::ADC_A_HL() {
    uint16_t address = (h << 8) | l;
    uint8_t val = bus->read(address) + getFlag(CARRY);
    setAddFlags(a, val);
    a += val;
    ++pc;
    cycleCount += 8;
}

/*  CE: ADC A, n
 *  A = A + (CY + n) */
void Cpu::ADC_A_n() {
    uint8_t n = bus->read(++pc);
    uint8_t cy = getFlag(CARRY);
    uint8_t val = n + cy;
    setAddFlags(a, val);
    a += val;
    ++pc;
    cycleCount += 8;
}

/*  SUB R1
 *  Subtract reg R1 from A. */
void Cpu::SUB_R1(uint8_t* R1) {
    setSubFlags(a, *R1);
    a -= *R1;
    ++pc;
    cycleCount += 4;
}

/*  96: SUB (HL)
 *  Subtract (HL) from A. */
void Cpu::SUB_HL() {
    uint16_t address = (h << 8) | l;
    uint8_t val = bus->read(address);
    setSubFlags(a, val);
    a -= val;
    ++pc;
    cycleCount += 8;
}

/*  D6: SUB n
 *  Subtract 8bit immed n from A */
void Cpu::SUB_n() {
    uint8_t val = bus->read(++pc);
    setSubFlags(a, val);
    a -= val;
    ++pc;
    cycleCount += 8;
}

/*  SBC A, R1
 *  Subtract value of R1 + carry from A. 
*   A = A - (R1 + CY) */
void Cpu::SBC_A_R1(uint8_t* R1) {
    uint8_t val = *R1 + getFlag(CARRY);
    setSubFlags(a, val);
    a -= val;
    ++pc;
    cycleCount += 4;
}

/*  SBC A, (HL)
 *  Subtract value of (HL) + carry from A. 
 *  A = A - ((HL) + CY) */
void Cpu::SBC_A_HL() {
    uint16_t address = (h << 8) | l;
    uint8_t val = bus->read(address) + getFlag(CARRY);
    setSubFlags(a, val);
    a -= val;
    ++pc;
    cycleCount += 8;
}

/*  AND R1
 *  Logical AND R1 with A, store result in A. */
void Cpu::AND_R1(uint8_t* R1) {
    a &= *R1;
    if (a == 0) {
        setFlag(ZERO);
    } else {
        resetFlag(ZERO);
    }
    resetFlag(SUB);
    setFlag(HALF_CARRY);
    resetFlag(CARRY);
    ++pc;
    cycleCount += 4;
}

/*  A6: AND (HL)
 *  Logical AND (HL) with A, store result in A. */
void Cpu::AND_HL() {
    uint16_t address = (h << 8) | l;
    uint8_t val = bus->read(address);
    a &= val;
    if (a == 0) {
        setFlag(ZERO);
    } else {
        resetFlag(ZERO);
    }
    resetFlag(SUB);
    setFlag(HALF_CARRY);
    resetFlag(CARRY);
    ++pc;
    cycleCount += 8;
}

/*  E6: AND n
 *  Logical AND n with A, store result in A. */
void Cpu::AND_n() {
    uint8_t val = bus->read(++pc);
    a &= val;
    if (a == 0) {
        setFlag(ZERO);
    } else {
        resetFlag(ZERO);
    }
    resetFlag(SUB);
    setFlag(HALF_CARRY);
    resetFlag(CARRY);
    ++pc;
    cycleCount += 8;
}

/*  OR R1
 *  Logical OR R1 with A, store result in A. */
void Cpu::OR_R1(uint8_t* R1) {
    a |= *R1;
    if (a == 0) {
        setFlag(ZERO);
    } else {
        resetFlag(ZERO);
    }
    resetFlag(SUB);
    resetFlag(HALF_CARRY);
    resetFlag(CARRY);
    ++pc;
    cycleCount += 4;
}

/*  B6: OR (HL)
 *  Logical OR (HL) with A, store result in A. */
void Cpu::OR_HL() {
    uint16_t address = (h << 8) | l;
    uint8_t val = bus->read(address);
    a |= val;
    if (a == 0) {
        setFlag(ZERO);
    } else {
        resetFlag(ZERO);
    }
    resetFlag(SUB);
    resetFlag(HALF_CARRY);
    resetFlag(CARRY);
    ++pc;
    cycleCount += 8;
}

/*  F6: OR n
 *  Logical AND n with A, store result in A. */
void Cpu::OR_n() {
    uint8_t val = bus->read(++pc);
    a |= val;
    if (a == 0) {
        setFlag(ZERO);
    } else {
        resetFlag(ZERO);
    }
    resetFlag(SUB);
    resetFlag(HALF_CARRY);
    resetFlag(CARRY);
    ++pc;
    cycleCount += 8;
}

/*  XOR R1
 *  A ^= R1 */
void Cpu::XOR_R1(uint8_t* R1) {
    a ^= *R1;

    if (a == 0) {
        setFlag(ZERO);
    } else {
        resetFlag(ZERO);
    }
    resetFlag(SUB);
    resetFlag(CARRY);
    resetFlag(HALF_CARRY);

    ++pc;
    cycleCount += 4;
}

/*  AE: XOR (HL)
 *  A ^= (HL) */
void Cpu::XOR_HL() {
    uint16_t address = (h << 8) | l;
    uint8_t val = bus->read(address);
    a ^= val;

    if (a == 0) {
        setFlag(ZERO);
    } else {
        resetFlag(ZERO);
    }
    resetFlag(SUB);
    resetFlag(CARRY);
    resetFlag(HALF_CARRY);

    ++pc;
    cycleCount += 8;
}

/*  EE: XOR n
 *  A ^= n */
void Cpu::XOR_n() {
    uint8_t val = bus->read(++pc);
    a ^= val;

    if (a == 0) {
        setFlag(ZERO);
    } else {
        resetFlag(ZERO);
    }
    resetFlag(SUB);
    resetFlag(CARRY);
    resetFlag(HALF_CARRY);

    ++pc;
    cycleCount += 8;
}

/*  CP R1
 *  Compare A with R1.
 *  To compare, the Game Boy subtracts A - R1, with the result 
 *  thrown away. If (A-R1) == 0 they are equal. */
void Cpu::CP_R1(uint8_t* R1) {
    setSubFlags(a, *R1);
    ++pc;
    cycleCount += 4;
}

/*  BE: CP (HL)
 *  Compare A with (HL).
 *  To compare, the Game Boy subtracts A - (HL), with the result 
 *  thrown away. If (A-(HL)) == 0 they are equal. */
void Cpu::CP_HL() {
    uint8_t val = bus->read(hl());
    setSubFlags(a, val);
    ++pc;
    cycleCount += 8;
}

/*  FE: CP n
 *  Compare A with n.
 *  To compare, the Game Boy subtracts A - n, with the result 
 *  thrown away. If (A-n) == 0 they are equal. */
void Cpu::CP_n() {
    uint8_t val = bus->read(++pc);
    setSubFlags(a, val);
    ++pc;
    cycleCount += 8;
}

/* INC R1
 * Increment register R1. */
void Cpu::INC_R1(uint8_t* R1) {
    setHalfCarryAdd(*R1, 1);
    (*R1)++;
    if (*R1 == 0) {
        setFlag(ZERO);
    } else {
        resetFlag(ZERO);
    }
    resetFlag(SUB);
    ++pc;
    cycleCount += 4;
}

/* 34: INC HL
 * Increment contents of (HL). */
void Cpu::INC_HL() {
    uint8_t val = bus->read(hl());
    setHalfCarryAdd(val, 1);
    ++val;
    bus->write(hl(), val);
    if (val == 0) {
        setFlag(ZERO);
    } else {
        resetFlag(ZERO);
    }
    resetFlag(SUB);
    ++pc;
    cycleCount += 12;
}

/* DEC R1
 * Increment register R1. */
void Cpu::DEC_R1(uint8_t* R1) {
    setHalfCarrySub(*R1, 1);
    --(*R1);
    if (*R1 == 0) {
        setFlag(ZERO);
    } else {
        resetFlag(ZERO);
    }
    setFlag(SUB);
    ++pc;
    cycleCount += 4;
}

/* 35: DEC HL
 * Increment contents of (HL). */
void Cpu::DEC_HL() {
    uint8_t val = bus->read(hl());
    setHalfCarrySub(val, 1);
    --val;
    bus->write(hl(), val);
    if (val == 0) {
        setFlag(ZERO);
    } else {
        resetFlag(ZERO);
    }
    setFlag(SUB);
    ++pc;
    cycleCount += 12;
}

// ========== JUMPS ==========
/*  C3: JP nn
 *  Jump to address nn */
void Cpu::JP_nn() {
    uint8_t lsb = bus->read(++pc);
    uint8_t msb = bus->read(++pc);
    pc = (msb << 8) | lsb;
    cycleCount += 12;
}

/* JP cc, nn
 * Jump to address nn if flag == condition */
void Cpu::JP_cc_nn(Flags flag, bool cond) {
    uint8_t lsb = bus->read(++pc);
    uint8_t msb = bus->read(++pc);
    uint8_t address = (msb << 8) | lsb;
    ++pc;
    if (flag == cond) {
        pc = address;
    }
    cycleCount += 12;
}

/* E9: JP (HL)
 * Jump to address (HL). */
void Cpu::JP_HL() {
    pc = bus->read(hl());
    cycleCount += 4;
}

/* 18: JR n
 * Add n to current pc. */
void Cpu::JR_n() {
    uint8_t val = bus->read(++pc);
    pc += val;
    cycleCount += 8;
}

/* JR cc, n
 * Add n to current address if flag == condition */
void Cpu::JR_cc_n(Flags flag, bool cond) {
    uint8_t val = bus->read(++pc);
    ++pc;
    if (flag == cond) {
        pc += val;
    }
    cycleCount += 8;
}
