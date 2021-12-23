
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "cpu.h"

#define CART_TYPE 0x147

/* Cpu::copyRom
 * Copies ROM from .gb file to memory.
 * MBC unimplemented. suuuuper basic.
 */
void Cpu::copyRom(std::string fname) {
    // Open rom for reading
    std::ifstream infile("gba/tetris.gb", std::ios::binary);
    if (!infile.is_open()) {
        std::cout << "ROM could not be opened" << std::endl;
    } else {
        std::cout << "ROM opened successfully" << std::endl;
    }

    // Copy rom into vector
    std::vector<uint8_t> rom(
         (std::istreambuf_iterator<char>(infile)),
         (std::istreambuf_iterator<char>()));
    infile.close();

    // Check and handle cartridge types
    // Only 1 type handled for now
    uint8_t ctype = rom.at(CART_TYPE);
    switch (ctype) {
        case 0x00:  mem = rom;
        default:    break;
    }
}

/* temp function
 * 
 */
void unknown(uint8_t opcode) {
    std::cout << "Unknown opcode found: " << std::hex << (int) opcode << std::endl;
}

/* Cpu::execute
 * Handles execution of all opcodes
 */
bool Cpu::execute() {
    uint8_t opcode = mem.at(pc);
    std::cout << "Opcode:" << std::hex << (int) opcode << std::endl;

    switch (opcode) {
        case 0x00: NOP();       break;
        case 0x32: LDD_HL_A();  break;
        case 0x06: LD_B_n();    break;
        case 0x0E: LD_C_n();    break;
        case 0x21: LD_HL_nn();  break;
        case 0xAF: XOR_a();     break;
        case 0xC3: JP_nn();     break;
        default:   unknown(opcode); return 1;
    }
    return 0;
};

// ================= OPCODES ================= //

// ===== MISC =====
/* NOP
 * No operation
 */
void Cpu::NOP() {
    pc += 1;
}

// ===== JUMPS =====
/* JP nn
 * Jump to address nn.
 * nn = two byte immediate value. (LS byte first.)
 */
void Cpu::JP_nn() {
    uint8_t lsb = mem.at(pc+1);
    uint8_t msb = mem.at(pc+2);
    pc = (msb << 8) | lsb;
    std::cout << std::hex << "\t JP_nn jumping to: " << (int) pc << std::endl;
}

// ===== 8-BIT ALU =====
/* XOR A
 * Reg A = A ^ A;
 * this xor always sets A to zero so don't even bother w the xor i guess
 */
void Cpu::XOR_a() {
    af &= 0x0080;   // Set A to 0 and set flags appropriately
    pc += 1;
    std::cout << "\t XOR A: new value of af is " << std::hex << (int) af << std::endl;
}

// ===== LOADS =====
/* LD HL, nn
 * Put value nn into HL
 */
void Cpu::LD_HL_nn() {
    uint8_t lsb = mem.at(pc+1);
    uint8_t msb = mem.at(pc+2);
    hl = (msb << 8) | lsb;
    pc += 3;
    std::cout << "\t HL = " << std::hex << (int) hl << std::endl;
}

/* LD B, n
 * Put value n into B
 */
void Cpu::LD_B_n() {
    uint8_t n = mem.at(pc+1);
    bc = (bc & 0x00FF) | (n << 8);
    std::cout << "\t BC = " << std::hex << (int) bc << std::endl;
    pc += 2;
}

/* LD C, n
 * Put value n into C 
 */
void Cpu::LD_C_n() {
    uint8_t n = mem.at(pc+1);
    bc = (bc & 0xFF00) | n;
    std::cout << "\t BC = " << std::hex << (int) bc << std::endl;
    pc += 2;
}

/* LD (HL-), A
 * Stores the contents of register A into the memory location specified by register pair HL, 
 * and decrements the contents of HL.
 */
void Cpu::LDD_HL_A() {
    uint16_t address = hl;
    std::cout << std::hex << (int) hl << std::endl;
    mem.at(address) = (af & 0xFF00) >> 8;
    hl--;
    pc += 1;
}