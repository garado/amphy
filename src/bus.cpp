
/* █▄▄ █░█ █▀ */ 
/* █▄█ █▄█ ▄█ */

#include <iomanip>
#include <stdio.h>
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

  // Open rom for reading
  // romFname = "src/bootix_dmg.bin";
  // std::ifstream infile(romFname, std::ios::binary);
  // 
  // if (!infile.is_open()) {
  //   printf("ROM could not be opened\n");
  //   return;
  // }

  // std::vector<u8> const bootRom(
  //    (std::istreambuf_iterator<char>(infile)),
  //    (std::istreambuf_iterator<char>()));
  // infile.close();

  // // Copy 0000-3FFF to bank 0
  // std::vector<u8>::const_iterator first = bootRom.begin();
  // std::vector<u8>::const_iterator last = bootRom.begin() + 0x3FFF;
  // rom_00.insert(rom_00.begin(), first, last);

  // cpu->pc = 0;
}

u8 Bus::Read(u16 address) const {
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
   
    if (address == JOYP) {
      if (cpu->joypSelection == JOYP_SEL_ACT_VAL) {
        // printf("   Return act: %02X\n", (cpu->keyvec_act & 0x0F) | JOYP_SEL_ACT_VAL);
        return cpu->keyvec_act;
        // return 0b00001111;
      } else if (cpu->joypSelection == JOYP_SEL_DIR_VAL) {
        // printf("   Return dir: %02X\n", (cpu->keyvec_dir & 0x0F) | JOYP_SEL_DIR_VAL);
        return cpu->keyvec_dir;
        // return 0b00001010;
      } else if (cpu->joypSelection == JOYP_SEL_BOTH_VAL) {
        return cpu->keyvec_dir | cpu->keyvec_act;
      } else {
        // printf("  Returning nothing\n");
        return 0xF0;
      }
    };

    return io_reg.at(address - IO_START);
  } else if (address < 0xFFFF) {
    return hram.at(address - HRAM_START);    
  } else if (address == 0xFFFF) {
    return int_enable;
  }

  return FAILURE;
}

void Bus::Write(u16 address, u8 val) {
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
  } else if (address < HRAM_START) {
    Write_MMIO(address, val);
  } else if (address < 0xFFFF) {
    hram.at(address - HRAM_START) = val;
  } else if (address == 0xFFFF) {
    int_enable = val;
  }
}

/* Special handling for writing to IO registers */
void Bus::Write_MMIO(u16 address, u8 val) {
  u8 curVal = Read(address);
  u8 mask, writeVal;
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
      if (val == JOYP_SEL_ACT_VAL) {
        cpu->joypSelection = JOYP_SEL_ACT_VAL;
      } else if (val == JOYP_SEL_DIR_VAL) {
        cpu->joypSelection = JOYP_SEL_DIR_VAL;
      } else if (val == JOYP_SEL_BOTH_VAL) {
        cpu->joypSelection = JOYP_SEL_BOTH_VAL;
      } else {
        cpu->joypSelection = JOYP_SEL_NIL_VAL;
      }
      break;

    // a write triggers DMA transfer
    // value is upper byte of start address
    case DMA:
      cpu->doDMATransfer = true;
      cpu->dmaAddr = val << 8;

      cpu->dmaByteCnt = 0;
      io_reg.at(shiftedAddr) = val;
      break;

    // Bit 7 pulled high
    case STAT:
      val |= 0x80;
      io_reg.at(shiftedAddr) = val;
      break;

    default:
      io_reg.at(shiftedAddr) = val;
      break;
  }
}

/* Bus::CopyRom
 * Copies ROM from .gb file to memory. */
u8 Bus::CopyRom(std::string fname) {
  // Open rom for reading
  romFname = fname;
  std::ifstream infile(romFname, std::ios::binary);
  
  if (!infile.is_open()) {
    printf("ROM could not be opened\n");
    return FAILURE;
  }

  std::vector<u8> const rom_(
     (std::istreambuf_iterator<char>(infile)),
     (std::istreambuf_iterator<char>()));
  infile.close();

  // Copy 0000-3FFF to bank 0
  std::vector<u8>::const_iterator first = rom_.begin();
  std::vector<u8>::const_iterator last = rom_.begin() + 0x3FFF;
  rom_00.insert(rom_00.begin(), first, last);

  // Copy 4000-7FFF to bank 1
  first = rom_.begin() + 0x4000;
  last = rom_.begin() + 0x7FFF;
  rom_01.insert(rom_01.begin(), first, last);

  cartType = rom_00.at(CART_TYPE);

  return SUCCESS;
}

/* Return a pointer to a memory value */
u8 * Bus::GetAddressPointer(u16 address)
{
  std::vector <u8>::iterator it;
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

void Bus::MBC(u16 addr, u8 value) {

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

    case CT_MBC3:
      break;

    default: break;
  }
}

void Bus::SwitchBanks(u8 bankNum) {
  // Open rom for reading
  std::ifstream infile(romFname, std::ios::binary);
  
  std::vector<u8> const romfile_(
     (std::istreambuf_iterator<char>(infile)),
     (std::istreambuf_iterator<char>()));
  infile.close();

  std::vector<u8>::const_iterator first = romfile_.begin();
  first += (bankNum * BANK_SIZE);

  std::vector<u8>::const_iterator last = romfile_.begin() + 0x3FFF;
  last += (bankNum * BANK_SIZE);

  rom_01.insert(rom_01.begin(), first, last);
}
