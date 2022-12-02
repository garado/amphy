
#include <iomanip>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "bus.h"

#define CART_TYPE 0x147

typedef enum CartType { 
  ROM, MBC1, MBC1_RAM, MBC1_RAM_BAT, MBC2 = 0x05, MBC2_BAT, 
  ROM_RAM = 0x08, ROM_RAM_BAT, MMM01 = 0x0B, MMM01_RAM, 
  MMM01_RAM_BAT, MBC3_TIM_BAT = 0x0F, MBC3_TIM_RAM_BAT,
  MBC3, MBC3_RAM, MBC3_RAM_BAT, MBC5 = 0x19, MBC5_RAM = 0x1A,
  MBC5_RAM_BAT, MBC5_RUM, MBC5_RUM_RAM, MBC5_RUM_RAM_BAT,
  MBC6 = 0x20, MBC7_SEN_RUM_RAM_BAR = 0x22
} CartType;

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

  std::cout << "COULDNT READ MEMORY" << std::endl;
  return 0;
}

/* Bus::write
 * Memory write function
 */
void Bus::write(uint16_t address, uint8_t val) {
  if (address <= 0x3FFF) {
    rom_00.at(address) = val;
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

/* Cpu::CopyRom
 * Copies ROM from .gb file to memory.
 * MBC unimplemented. suuuuper basic.
 */
int8_t Bus::CopyRom(std::string fname) {
  // Open rom for reading
  std::ifstream infile("gba/bgbtest.gb", std::ios::binary);
  if (!infile.is_open()) {
    std::cout << "ROM could not be opened" << std::endl;
    return -1;
  }

  // Copy rom into vector
  std::vector<uint8_t> const rom_(
     (std::istreambuf_iterator<char>(infile)),
     (std::istreambuf_iterator<char>()));
  infile.close();

  // === Check and handle cartridge types ===
  // Only 1 type handled for now

  uint8_t ctype = rom_.at(CART_TYPE);

  if (ctype == ROM) { // ROM
    std::vector<uint8_t>::const_iterator first = rom_.begin();
    std::vector<uint8_t>::const_iterator last = rom_.begin() + 0x3FFF;
    rom_00.insert(rom_00.begin(), first, last);
    first = rom_.begin() + 0x4000;
    last = rom_.begin() + 0x7FFF;
    rom_01.insert(rom_01.begin(), first, last);
  } else if (ctype == MBC1) { // MBC: Not working yet
    std::vector<uint8_t>::const_iterator first = rom_.begin();
    std::vector<uint8_t>::const_iterator last = rom_.begin() + 0x3FFF;
    rom_00.insert(rom_00.begin(), first, last);
    first = rom_.begin() + 0x4000;
    last = rom_.begin() + 0x7FFF;
    rom_01.insert(rom_01.begin(), first, last);
  }

  /* // Print copied rom contents
  int count = 0;
  int totalCnt = 0;
  for (int i : rom_00) {
    if (count % 4 == 0) {
      count = 0;
      if (count != 0) {
        std::cout << std::endl;
      }
      std::cout << "0x" << std::setfill('0') << std::hex << std::setw(2) << totalCnt << ": ";
      totalCnt += 4;
    }
    std::cout << std::hex << (int) i << ' ';
    ++count;
  }
  */

  return 1;
}
