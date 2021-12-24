
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "cpu.h"

#define CART_TYPE 0x147

/* Cpu::flag
 * Gets and sets flags. */
uint8_t Cpu::flag(Flags flag) {
    uint8_t f = af & 0x00FF;
    return (f & (1 << flag)) >> flag;
}

void Cpu::flag(Flags flag, bool val) {
    uint8_t f = af & 0x00FF;
    f &= ~(1 << flag);  // set bit to 0
    f |= (val << flag); // so we can set bit to val
    af = (af & 0xFF00) | f;
}


/* Cpu::copyRom
 * Copies ROM from .gb file to memory.
 * MBC unimplemented. suuuuper basic.
 */
void Cpu::copyRom(std::string fname) {
    // Open rom for reading
    std::ifstream infile("gba/tetris.gb", std::ios::binary);
    if (!infile.is_open()) {
        std::cout << "ROM could not be opened" << std::endl;
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

void Cpu::regdump() {
    std::cout << "af: " << std::hex << (int) af << std::endl;
    std::cout << "bc: " << std::hex << (int) bc << std::endl;
    std::cout << "de: " << std::hex << (int) de << std::endl;
    std::cout << "hl: " << std::hex << (int) hl << std::endl;
    std::cout << "sp: " << std::hex << (int) sp << std::endl;
    std::cout << "pc: " << std::hex << (int) pc << std::endl;

    std::cout << "opcode: " << std::hex << (int) mem(pc) << std::endl;
}

/* temp function
 * 
 */
void Cpu::unknown(uint8_t opcode) {
    std::cout << "Unknown opcode found: " << std::hex << (int) opcode << std::endl;
    regdump();
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
 * Handles execution of all opcodes */
bool Cpu::cycle() {
    uint8_t opcode = mem(pc);
    switch (opcode) {
        case 0x00: NOP();       break;
        case 0x05: DEC_b();     break;
        case 0x06: LD_B_n();    break;
        case 0x0D: DEC_c();     break;
        case 0x0E: LD_C_n();    break;
        case 0x20: JR_NZ_n();   break;
        case 0x21: LD_HL_nn();  break;
        case 0x32: LDD_HL_A();  break;
        case 0x3E: LD_A_n();    break;
        case 0xAF: XOR_a();     break;
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
}

/* F3: DI
 *  Reset IME flag and prohibit maskable interrupts */
void Cpu::DI() {
    // ???
    pc += 1;
}

// ========== JUMPS ==========
/* C3: JP nn
 * Jump to address nn.
 * nn = two byte immediate value. (LS byte first.) */
void Cpu::JP_nn() {
    uint8_t lsb = mem(++pc);
    uint8_t msb = mem(++pc);
    pc = (msb << 8) | lsb;
}

/* 20: JR NZ, n
 * If zero flag == 0 add n to current address and jump to it */
void Cpu::JR_NZ_n() {
    int8_t val = mem(++pc);
    ++pc;
    if (flag(ZERO) == false) {
        pc += val;
    }
}

// ========== 8-BIT ALU ==========
/* AF: XOR A
 * Reg A = A ^ A;
 * this xor always sets A to zero so don't even bother w the xor i guess */
void Cpu::XOR_a() {
    af = 0x0080;   // Set A to 0 and sets flags accordingly
    pc += 1;
}

/* 05: DEC B
 * Decrement value in reg B by 1 */
void Cpu::DEC_b() {
    uint8_t b_prev = (bc & 0xFF00) >> 8;
    uint8_t b_new = b_prev - 1;
    bc = (bc & 0x00FF) | (b_new << 8);

    // Set flags
    flag(SUB, true);
    if (b_new == 0) {
        flag(ZERO, true);
    } else {
        flag(ZERO, false);
    }
    if ((b_prev & 15) + (1 & 15) > 15) {
        flag(HALF_CARRY, true);
    } else {
        flag(HALF_CARRY, false);
    }

    pc += 1;
}

/* 0D: DEC C
 * Decrement value in reg C by 1 */
void Cpu::DEC_c() {
    uint8_t c_prev = (bc & 0xFF);
    uint8_t c_new = c_prev - 1;
    bc = (bc & 0xFF00) | c_new;

    // Set flags
    flag(SUB, true);
    if (c_new == 0) {
        flag(ZERO, true);
    } else {
        flag(ZERO, false);
    }
    if ((c_prev & 15) + (1 & 15) > 15) {
        flag(HALF_CARRY, true);
    } else {
        flag(HALF_CARRY, false);
    }

    pc += 1;
}

// ========== LOADS ==========
/* 06:LD B, n
 * Put immediate value n into B */
void Cpu::LD_B_n() {
    uint8_t n = mem(pc+1);
    bc = (bc & 0x00FF) | (n << 8);
    pc += 2;
}

/* 0E: LD C, n
 * Put immediate value n into C */
void Cpu::LD_C_n() {
    uint8_t n = mem(pc+1);
    bc = (bc & 0xFF00) | n;
    pc += 2;
}

/* 21: LD HL, nn
 * Put value nn into HL */
void Cpu::LD_HL_nn() {
    uint8_t lsb = mem(pc+1);
    uint8_t msb = mem(pc+2);
    hl = (msb << 8) | lsb;
    pc += 3;
}

/* 32: LD (HL-), A
 * Stores the contents of register A into the memory location specified by register pair HL, 
 * and decrements the contents of HL. */
void Cpu::LDD_HL_A() {
    uint16_t address = hl;
    uint8_t val = (af & 0xFF00) >> 8;
    mem(address, val);
    hl--;
    pc += 1;
}

/* 3E: LD A, n
 * Put immediate value n into A */
void Cpu::LD_A_n() {
    uint8_t n = mem(pc+1);
    af = (af & 0x00FF) | (n << 8);
    pc += 2;
}

/* E0: LDH (n), A
 * Put A into memory address $FF00 + n. */
void Cpu::LDH_n_A() {
    uint16_t address = 0xFF00 | mem(pc+1);
    uint8_t a = (af & 0xFF00) >> 8;
    mem(address, a);
    pc += 2;
}

/* F0: LDH (A), n
 * Put value in memory address $FF00 + n into A. */
void Cpu::LDH_A_n() {
    uint16_t n = mem(pc+1);
    uint16_t address = 0xFF00 | n;
    uint8_t val = mem(address);
    af = (af & 0x00FF) | (val << 8);
    pc += 2;
}
