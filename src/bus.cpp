
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "bus.h"

#define CART_TYPE 0x147

/* Bus::read
 * Memory read function
 */
uint8_t Bus::read(uint16_t address) const {
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

/* Bus::write
 * Memory write function
 */
void Bus::write(uint16_t address, uint8_t val) {
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

/* Cpu::copyRom
 * Copies ROM from .gb file to memory.
 * MBC unimplemented. suuuuper basic.
 */
void Bus::copyRom(std::string fname) {
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
