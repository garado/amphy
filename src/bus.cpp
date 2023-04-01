
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
  cartType = rom_00.at(CART_TYPE);
  io_reg.at(TAC  - IO_START) = 0xF8;
  io_reg.at(LCDC - IO_START) = 0x00;
  io_reg.at(JOYP - IO_START) = 0xCF;
  // io_reg.at(INTE - IO_START) = 0xE0;
  // io_reg.at(INTF - IO_START) = 0xE0;
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
    // fprintf(stderr, "Warning: Invalid memory region read: 0x%04x\n", address);
    return 0xFF;
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
  if (address == TIMA && cpu->doneTMAreload) return;

  if (address < ROM1_START) {
    MBC(address, val);
  } else if (address < VRAM_START) {
    MBC(address, val);
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
    // fprintf(stderr, "Warning: Invalid memory region write: 0x%02x to 0x%04x\n", val, address);
  } else if (address < HRAM_START) {
    Write_MMIO(address, val);
  } else if (address < 0xFFFF) {
    hram.at(address - HRAM_START) = val;
  } else if (address == 0xFFFF) {
    int_enable = val;
  }
}

/* Special handling for writing to IO registers */
void Bus::Write_MMIO(uint16_t address, uint8_t val) {
  uint8_t curVal = Read(address);
  uint8_t mask;
  Address shiftedAddr = address - IO_START;

  switch (address) {

    // case INTE: // only lower 5 bits writable
    // case INTF:
    //   io_reg.at(shiftedAddr) = curVal | (val & 0x1F);
    //   break;

    case DIV: // any write clears
      io_reg.at(shiftedAddr) = 0;
      cpu->sysclk = 0;
      break;

    case TAC: // only bit 0-2 writable
      io_reg.at(shiftedAddr) = curVal | (val & 0b111);
      break;

    /* Writing to this changes what bits 0-3 represent
     * Write 0 to bit 4: bit0-3 represents direction btn state
     * Write 0 to bit 5: bit0-3 represents action btn state
     * All other bits are read only */
    case JOYP:
      // Clear then update select bits
      mask = 0b00110000;
      curVal = (curVal & ~mask) | (mask & val);

      // Update state bits
      mask = 0xF0;
      if (~BIT_GET(val, JOYP_SEL_ACTION)) {
        curVal = (curVal & mask) | (cpu->keyvec_act & ~mask);
      } else if (~BIT_GET(val, JOYP_SEL_DIR)) {
        curVal = (curVal & mask) | (cpu->keyvec_dir & ~mask);
      }

      io_reg.at(shiftedAddr) = curVal;
      break;

    default:
      io_reg.at(shiftedAddr) = val;
      break;
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

  // Copy 0000-3FFF to bank 0
  std::vector<uint8_t>::const_iterator first = rom_.begin();
  std::vector<uint8_t>::const_iterator last = rom_.begin() + 0x3FFF;
  rom_00.insert(rom_00.begin(), first, last);

  // Copy 4000-7FFF to bank 1
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

void Bus::MBC(uint16_t addr, uint8_t value) {

  switch (cartType) {
    case CT_ROM_ONLY: break;

    case CT_MBC1:
      if (addr < 0x1FFF) {
        ramEnable = value == 0x0A; 
      } else if (addr < 0x3FFF) {
        value &= 0x1F; // only lower 5 bits used
        if (value == 0) value = 1;
        SwitchBanks(value);
      }
      break;

    default: break;
  }
}

void Bus::SwitchBanks(uint8_t bankNum) {
  // Open rom for reading
  std::ifstream infile(romFname, std::ios::binary);
  
  std::vector<uint8_t> const romfile_(
     (std::istreambuf_iterator<char>(infile)),
     (std::istreambuf_iterator<char>()));
  infile.close();

  std::vector<uint8_t>::const_iterator first = romfile_.begin();
  first += (bankNum * BANK_SIZE);

  std::vector<uint8_t>::const_iterator last = romfile_.begin() + 0x3FFF;
  last += (bankNum * BANK_SIZE);

  rom_01.insert(rom_01.begin(), first, last);
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
