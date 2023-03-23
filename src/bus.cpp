
/* █▄▄ █░█ █▀ */ 
/* █▄█ █▄█ ▄█ */

#include <iomanip>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "utils/debug.h"
#include "bus.h"
#include "defines.h"

#define CART_TYPE 0x147

// Enum of different Gameboy cartridge types
typedef enum CartType { 
  ROM,
  MBC1,
  MBC1_RAM,
  MBC1_RAM_BAT,
  MBC2 = 0x05,
  MBC2_BAT,
  ROM_RAM = 0x08,
  ROM_RAM_BAT,
  MMM01 = 0x0B,
  MMM01_RAM,
  MMM01_RAM_BAT,
  MBC3_TIM_BAT = 0x0F,
  MBC3_TIM_RAM_BAT,
  MBC3,
  MBC3_RAM,
  MBC3_RAM_BAT,
  MBC5 = 0x19,
  MBC5_RAM = 0x1A,
  MBC5_RAM_BAT,
  MBC5_RUM,
  MBC5_RUM_RAM,
  MBC5_RUM_RAM_BAT,
  MBC6 = 0x20,
  MBC7_SEN_RUM_RAM_BAR = 0x22
} CartType;

/* Bus::init
 * Initializes registers to their post-boot rom values */
void Bus::init() {
  allow_div = true;
  write(DIV, 0x1E);
  allow_div = false;
}

/* Bus::read
 * Memory read function
 * @param address The address to read from */
uint8_t Bus::read(uint16_t address) const {
  // Temporary for GBDoc
  if (address == 0xFF44) {
    return 0x90;
  }

  if (address < ROM1_START) {
    return rom_00.at(address);
  } else if (address < VRAM_START) {
    return rom_01.at(address - ROM1_START);
  } else if (address < EXTRAM_START) {
    return vram.at(address - VRAM_START);
  } else if (address < WRAM0_START) {
    return ext_ram.at(address - EXTRAM_START);
  } else if (address < WRAM1_START) {
    return wram_0.at(address - WRAM0_START);
  } else if (address < ECHRAM_START) {
    return wram_1.at(address - WRAM1_START);
  } else if (address < OAM_START) {
    return ech_ram.at(address - ECHRAM_START);
  } else if (address < INVALID_START) {
    return oam.at(address - OAM_START);
  } else if (address < IO_START) {
    std::cout << "INVALID MEMORY REGION READ" << std::endl;
  } else if (address < HRAM_START) {
    return io_reg.at(address - IO_START);
  } else if (address < 0xFFFF) {
    return hram.at(address - HRAM_START);    
  } else if (address == 0xFFFF) {
    return int_enable;
  }

  std::cout << __PRETTY_FUNCTION__ << ": Couldn't read memory" << std::endl;
  return FAILURE;
}

/* @Function Bus::write
 * @param address Address to write to
 * @param val Value to store into address */
void Bus::write(uint16_t address, uint8_t val) {
  // Any write to DIV timer sets it to 0
  if (!allow_div && address == DIV) val = 0;

  // Upper 5 TAC bits are pulled up to 1
  if (address == TAC) val |= 0b11111000;

  if (address < ROM1_START) {
    rom_00.at(address) = val;
  } else if (address < VRAM_START) {
    rom_01.at(address - ROM1_START) = val;
  } else if (address < EXTRAM_START) {
    vram.at(address - VRAM_START) = val;
  } else if (address < WRAM0_START) {
    ext_ram.at(address - EXTRAM_START) = val;
  } else if (address < WRAM1_START) {
    wram_0.at(address - WRAM0_START) = val;
  } else if (address < ECHRAM_START) {
    wram_1.at(address - WRAM1_START) = val;
  } else if (address < OAM_START) {
    ech_ram.at(address - ECHRAM_START) = val;
  } else if (address < INVALID_START) {
    oam.at(address - OAM_START) = val;
  } else if (address < IO_START) {
    std::cout << "INVALID MEMORY REGION READ" << std::endl;
  } else if (address < HRAM_START) {
    io_reg.at(address - IO_START) = val;
  } else if (address < 0xFFFF) {
    hram.at(address - HRAM_START) = val;    
  } else if (address == 0xFFFF) {
    int_enable = val;
  }
}

/* Cpu::CopyRom
 * Copies ROM from .gb file to memory.
 * MBC unimplemented. suuuuper basic.
 */
uint8_t Bus::CopyRom(std::string fname) {
  // Open rom for reading
  std::ifstream infile(fname, std::ios::binary);
  
  if (!infile.is_open()) {
    std::cout << "ROM could not be opened" << std::endl;
    return FAILURE;
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

  return SUCCESS;
}

// Return a pointer to a memory value
uint8_t * Bus::GetAddressPointer(uint16_t address)
{
  std::vector <uint8_t>::iterator it;
  if (address <= 0x3FFF) {
    it = rom_00.begin();
  } else if (address <= 0x7FFF) {
    it = rom_01.begin() + (address - 0x4000);
  } else if (address <= 0x9FFF) {
    it = vram.begin() + (address - 0x8000);
  } else if (address <= 0xBFFF) {
    it = ext_ram.begin() + (address - 0xA000);
  } else if (address <= 0xCFFF) {
    it = wram_0.begin() + (address - 0xC000);
  } else if (address <= 0xDFFF) {
    it = wram_1.begin() + (address - 0xD000);
  } else if (address <= 0xFDFF) {
    it = ech_ram.begin() + (address - 0xE000);
  } else if (address <= 0xFE9F) {
    it = oam.begin() + (address - 0xFE00);
  } else if (address <= 0xFF7F) {
    it = io_reg.begin() + (address - 0xFF00);
  } else if (address <= 0xFFFE) {
    it = hram.begin() + (address - 0xFF80);
  } else if (address <= 0xFFFF) {
    return &int_enable;
  }
  return &(*it);
}
