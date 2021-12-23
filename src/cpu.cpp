
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
    std::vector<uint8_t> const rom_(
         (std::istreambuf_iterator<char>(infile)),
         (std::istreambuf_iterator<char>()));
    infile.close();

    // Check and handle cartridge types
    // Only 1 type handled for now
    uint8_t ctype = rom_.at(CART_TYPE);

    if (ctype == 0x00) { // ROM
        std::vector<uint8_t>::const_iterator first = rom_.begin();
        std::vector<uint8_t>::const_iterator last = rom_.begin() + 0x3FFF;
        rom_00.insert(rom_00.begin(), first, last);
        first = rom_.begin() + 0x4000;
        last = rom_.begin() + 0x7FFF;
        rom_01.insert(rom_01.begin(), first, last);
    }
}

/* temp function
 * 
 */
void unknown(uint8_t opcode) {
    std::cout << "Unknown opcode found: " << std::hex << (int) opcode << std::endl;
}

/* Cpu::mem
 * Memory read function
 */
uint8_t Cpu::mem(uint16_t address) {
    if (address <= 0x3FFF) {
        return rom_00.at(address);
    } else if (address <= 0x7FFF) {
        // SWITCHABLE BANK NOT IMPLEMENTED
        return rom_01.at(address - 0x4000);
    } else if (address <= 0x9FFF) {
        // SWITCHABLE BANK NOT IMPLEMENTED
        return vram.at(address - 0x8000);
    } else if (address <= 0xBFFF) {
        // SWITCHABLE BANK NOT IMPLEMENTED
        return ext_ram.at(address - 0xA000);
    } else if (address <= 0xCFFF) {
        return wram_0.at(address - 0xC000);
    } else if (address <= 0xDFFF) {
        // SWITCHABLE BANK NOT IMPLEMENTED
        return wram_1.at(address - 0xD000);
    } else if (address <= 0xFDFF) {
        return ech_ram.at(address - 0xE000);
    } else if (address <= 0xFE9F) {
        return oam.at(address - 0xFE00);
    } else if (address <= 0xFF7F) {
        return io_reg.at(address - 0xFF00);
    } else if (address <= 0xFFFE) {
        return hram.at(address - 0xFF80);        
    } else if (address <= 0xFFFF) {
        return int_enable;
    }
}

/* Cpu::mem
 * Memory write function
 */
void Cpu::mem(uint16_t address, uint8_t val) {
    if (address <= 0x3FFF) {
        rom_00.at(address);
    } else if (address <= 0x7FFF) {
        // SWITCHABLE BANK NOT IMPLEMENTED
        rom_01.at(address - 0x4000) = val;
    } else if (address <= 0x9FFF) {
        // SWITCHABLE BANK NOT IMPLEMENTED
        vram.at(address - 0x8000) = val;
    } else if (address <= 0xBFFF) {
        // SWITCHABLE BANK NOT IMPLEMENTED
        ext_ram.at(address - 0xA000) = val;
    } else if (address <= 0xCFFF) {
        wram_0.at(address - 0xC000) = val;
    } else if (address <= 0xDFFF) {
        // SWITCHABLE BANK NOT IMPLEMENTED
        wram_1.at(address - 0xD000) = val;
    } else if (address <= 0xFDFF) {
        ech_ram.at(address - 0xE000) = val;
    } else if (address <= 0xFE9F) {
        oam.at(address - 0xFE00) = val;
    } else if (address <= 0xFF7F) {
        io_reg.at(address - 0xFF00) = val;
    } else if (address <= 0xFFFE) {
        hram.at(address - 0xFF80) = val;        
    } else if (address <= 0xFFFF) {
        int_enable = val;
    }
}

/* Cpu::execute
 * Handles execution of all opcodes
 */
bool Cpu::execute() {
    uint8_t opcode = mem(pc);
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
    uint8_t lsb = mem(pc+1);
    uint8_t msb = mem(pc+2);
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
    uint8_t lsb = mem(pc+1);
    uint8_t msb = mem(pc+2);
    hl = (msb << 8) | lsb;
    pc += 3;
    std::cout << "\t HL = " << std::hex << (int) hl << std::endl;
}

/* LD B, n
 * Put value n into B
 */
void Cpu::LD_B_n() {
    uint8_t n = mem(pc+1);
    bc = (bc & 0x00FF) | (n << 8);
    std::cout << "\t BC = " << std::hex << (int) bc << std::endl;
    pc += 2;
}

/* LD C, n
 * Put value n into C 
 */
void Cpu::LD_C_n() {
    uint8_t n = mem(pc+1);
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
    uint8_t val = (af & 0xFF00) >> 8;
    mem(address, val);
    hl--;
    pc += 1;
}