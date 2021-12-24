
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "cpu.h"

/* Cpu::execute
 * Handles execution of all opcodes */
bool Cpu::execute() {
    uint8_t opcode = mem(pc);

    switch (opcode) {
        case 0x00: NOP();       break;
        case 0x05: DEC_n(&b);   break;
        case 0x06: LD_R_n(&b);  break;
        case 0x0D: DEC_n(&c);   break;
        case 0x0E: LD_R_n(&c);  break;
        case 0x20: JR_cc_n(ZERO, 0);  break;
        case 0x21: LD_RR_nn(&h, &l);  break;
        case 0x32: LDD_RR_n(&h, &l);  break;
        case 0x3E: LD_R_n(&a);  break;
        case 0xAF: XOR_n(&a);   break;
        case 0xC3: JP_nn();     break;
        case 0xE0: LDH_n_A();   break;
        case 0xF0: LDH_A_n();   break; 
        case 0xF3: DI();        break;
        default:   
            unknown(opcode); 
            return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
};

// ================= OPCODES ================= //

// ===== MISC =====
/* 00: NOP
 * No operation. */
void Cpu::NOP() {
    pc += 1;
    cycleCount += 4;
}

/* F3: DI
 *  Reset IME flag and prohibit maskable interrupts */
void Cpu::DI() {
    // ???
    pc += 1;
    cycleCount += 4;
}

// ========== 8-BIT ALU ==========

/* DEC n
 * Decrement reg*/
void Cpu::DEC_n(uint8_t* reg) {
    uint8_t prev = *reg;
     (*reg)--;
    
    flag(SUB, true);

    if (*reg == 0) {
        flag(ZERO, true);
    } else {
        flag(ZERO, false);
    }

    // might be wrong
    if ((prev & 15) - (1 & 15) < 0) {
        flag(HALF_CARRY, true);
    } else {
        flag(HALF_CARRY, false);
    }

    ++pc;
    cycleCount += 4;
}

/* XOR n
 * a = reg ^ a */
void Cpu::XOR_n(uint8_t* reg) {
    a = a ^ *reg;
    f = 0x00;
    if (a == 0) {
        flag(ZERO, true);
    } else {
        flag(ZERO, false);
    }
    pc += 1;
    cycleCount += 4;
}

// ========== JUMPS ==========
/* C3: JP nn
 * Jump to address nn.
 * nn = two byte immediate value. (LS byte first.) */
void Cpu::JP_nn() {
    uint8_t lsb = mem(++pc);
    uint8_t msb = mem(++pc);
    pc = (msb << 8) | lsb;
    cycleCount += 16;
}

/* JR cc, n
 * If flag == b, add 8-bit immediate value n to current address
 * and jump to it. Otherwise go to next address as usual. */
void Cpu::JR_cc_n(Flags f, bool b) {
    int8_t n = mem(++pc);
    ++pc;
    if (flag(f) == b) {
        pc += (uint16_t) n;
    }
    cycleCount += 8;
}

// ========== LOADS ==========
/* LD REG, n
 * Load 8-bit immed operand n into reg */
void Cpu::LD_R_n(uint8_t* reg) {
    uint8_t n = mem(++pc);
    *reg = n;
    ++pc;
}

/* LD REG, d16
 * Load 2 bytes of immediate data into register pair.
 * First byte of data is the ls byte. */
void Cpu::LD_RR_nn(uint8_t* reg1, uint8_t* reg2) {
    *reg2 = mem(++pc);
    *reg1 = mem(++pc);
    cycleCount += 12;
    ++pc;
}

/* LDD RR, n
 * Store contents of reg A into memory location specified by register pair
 * and decrement the contents of reg pair. */
void Cpu::LDD_RR_n(uint8_t* reg1, uint8_t* reg2) {
    // Store contents
    uint16_t address = (*reg1 << 8) | *reg2;
    uint8_t val = a;
    mem(address, val);

    // Decrement reg pair
    address--;
    *reg1 = address >> 8;
    *reg2 = (address & 0x00FF);
    ++pc;
}

/* E0: LDH (n), A
 * Put A into memory address $FF00 + n. */
void Cpu::LDH_n_A() {
    uint16_t address = 0xFF00 | mem(++pc);
    mem(address, a);
    ++pc;
}

/* F0: LDH (A), n
 * Put value in memory address $FF00 + n into A. */
void Cpu::LDH_A_n() {
    uint16_t address = 0xFF00 | mem(++pc);
    a = mem(address);
    ++pc;
}