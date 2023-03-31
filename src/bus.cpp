
/* █▄▄ █░█ █▀ */ 
/* █▄█ █▄█ ▄█ */

#include <iomanip>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "debug.h"
#include "bus.h"
#include "cpu.h"
#include "common.h"

bool testflag;

void Bus::Init() {
  allow_div = true;
  Write(DIV, 0x1E);
  Write(LCDC, 0x91); //??
  allow_div = false;
}

uint8_t Bus::Read(uint16_t address) const {
  // Temporary for GBDoc
  // if (address == 0xFF44) return 0x90;

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
    return 0xFF;
    fprintf(stderr, "INVALID MEMORY REGION READ: 0x%4x\n", address);
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

void Bus::Write(uint16_t address, uint8_t val) {
  // Any write to DIV timer sets it to 0
  if (!allow_div && address == DIV) {
    val = 0;
    cpu->sysclk = 0;
  }

  // Upper 5 TAC bits are pulled up to 1
  if (address == TAC) val |= 0b11111000;

  if (address < ROM1_START) {
    // MBC: RAM enable
    if (address < 0x1FFF) {
      ramEnable = (val & 0b1111) == 0xA;
    }

    // MBC: ROM bank number
    else if (address < 0x3FFF) {
      if (val == 0) val = 1;
      SwitchBanks(val);
    }

  } else if (address < VRAM_START) {

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
    fprintf(stderr, "INVALID MEMORY REGION WRITE: 0x%2.0x to 0x%4.0x\n", val, address);
  } else if (address < HRAM_START) {
    io_reg.at(address - IO_START) = val;
  } else if (address < 0xFFFF) {
    hram.at(address - HRAM_START) = val;    
  } else if (address == 0xFFFF) {
    int_enable = val;
  }
}

/* Bus::CopyRom
 * Copies ROM from .gb file to memory. */
uint8_t Bus::CopyRom(std::string fname) {
  // Open rom for reading
  romFname = fname;
  std::ifstream infile(romFname, std::ios::binary);
  
  if (!infile.is_open()) {
    std::cout << "ROM could not be opened" << std::endl;
    return FAILURE;
  }

  std::vector<uint8_t> const rom_(
     (std::istreambuf_iterator<char>(infile)),
     (std::istreambuf_iterator<char>()));
  infile.close();

  std::vector<uint8_t>::const_iterator first = rom_.begin();
  std::vector<uint8_t>::const_iterator last = rom_.begin() + 0x3FFF;
  rom_00.insert(rom_00.begin(), first, last);
  first = rom_.begin() + 0x4000;
  last = rom_.begin() + 0x7FFF;
  rom_01.insert(rom_01.begin(), first, last);

  cartType = rom_00.at(CART_TYPE);

  return SUCCESS;
}

/* Return a pointer to a memory value */
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

/* Maybe these should be moved elsewhere (utils?) */
uint8_t const Bus::BitTest(uint16_t address, uint8_t bitpos) {
  return (Read(address) & (1 << bitpos));
}

void Bus::BitSet(uint16_t address, uint8_t bitpos) {
  uint8_t data = Read(address);
  data |= (1 << bitpos);
  Write(address, data);
}

void Bus::BitClear(uint16_t address, uint8_t bitpos) {
  uint8_t mask = ~(1 << bitpos);
  uint8_t data = Read(address) & mask;
  Write(address, data);
}

void Bus::SwitchBanks(uint8_t bankNum) {
  // Open rom for reading
  std::ifstream infile(romFname, std::ios::binary);
  
  std::vector<uint8_t> const romfile_(
     (std::istreambuf_iterator<char>(infile)),
     (std::istreambuf_iterator<char>()));
  infile.close();

  std::vector<uint8_t>::const_iterator first = romfile_.begin();
  first += (bankNum * 0x2000);

  std::vector<uint8_t>::const_iterator last = romfile_.begin() + 0x3FFF;
  last += (bankNum * 0x2000);

  rom_01.insert(rom_01.begin(), first, last);

  cartType = rom_00.at(CART_TYPE);
}
