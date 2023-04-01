
#include <stdio.h>
#include <iostream>
#include <vector>

#ifndef BUS_H
#define BUS_H

// For MBC (CT == Cart Type)
#define CART_TYPE 0x0147
#define BANK_SIZE 0x2000 

#define CT_ROM_ONLY         0x00 // done
#define CT_MBC1             0x01 // done
#define CT_MBC1_RAM         0x02
#define CT_MBC1_RAM_BAT     0x03
#define CT_MBC2             0x05
#define CT_MBC2_BAT         0x06
#define CT_ROM_RAM_1        0x08
#define CT_ROM_RAM_BAT_1    0x09
#define CT_MMM01            0x0B
#define CT_MMM01_RAM        0x0C
#define CT_MMM01_RAM_BAT    0x0D
#define CT_MBC3_TMR_BAT     0x0F
#define CT_MBC3_TMR_RAM_BAT_2 0x10
#define CT_MBC3             0x11
#define CT_MBC3_RAM_2       0x12
#define CT_MBC3_RAM_BAT_2   0x13
#define CT_MBC5             0x19
#define CT_MBC5_RAM         0x1A
#define CT_MBC5_RAM_BAT     0x1B
#define CT_MBC5_RUM         0x1C
#define CT_MBC5_RUM_RAM     0x1D
#define CT_MBC5_RUM_RAM_BAT 0x1E
#define CT_MBC6             0x20
#define CT_MBC7_SENSOR_RUM_RAM_BAT 0x22
#define CT_POCKET CAMERA    0xFC
#define CT_BANDAI TAMA5     0xFD
#define CT_HuC3             0xFE
#define CT_HuC1_RAM_BAT     0xFF


// Starting address for different regions of memory
#define ROM0_START    0x0000 // 0000-3FFF
#define ROM1_START    0x4000 // 4000-7FFF
#define VRAM_START    0x8000 // 8000-9FFF
#define EXTRAM_START  0xA000 // A000-BFFF
#define WRAM0_START   0xC000 // C000-CFFF
#define WRAM1_START   0xD000 // D000-DFFF
#define ECHRAM_START  0xE000 // E000-FDFF
#define OAM_START     0xFE00 // FE00-FE9F
#define INVALID_START 0xFEA0
#define IO_START      0xFF00 // FF00-FF7F
#define HRAM_START    0xFF80 // FF80-FFFE

enum mbcModeSelect {
  ROM_MODE,
  RAM_MODE
};

class Cpu;

class Bus
{
  private:
    // Memory map
    // Rom bank 0. Fixed. 0000 - 3FFF
    std::vector<uint8_t> rom_00 = std::vector<uint8_t>(16384);
    
    // Rom bank 1. Switchable. 4000 - 7FFF
    std::vector<uint8_t> rom_01 = std::vector<uint8_t>(16384);
    
    // Video ram. 8000-9FFF
    std::vector<uint8_t> vram = std::vector<uint8_t>(8192);
    
    // From cart. Switchable. A000 - BFFF
    std::vector<uint8_t> ext_ram = std::vector<uint8_t>(8192);
    
    // C000 - CFFF
    std::vector<uint8_t> wram_0 = std::vector<uint8_t>(4096);
    
    // D000 - DFFF
    std::vector<uint8_t> wram_1 = std::vector<uint8_t>(4096);
    
    // Mirror of C000-DFFF. E000-FDFF
    std::vector<uint8_t> ech_ram = std::vector<uint8_t>(7680);
    
    // FE00-FE9F
    // This is the CPU's copy of OAM. In the actual hardware, a
    // DMA transfer copies this data to the PPU's OAM.
    std::vector<uint8_t> oam = std::vector<uint8_t>(160);
    
    // IO registers FF00-FF7F
    std::vector<uint8_t> io_reg = std::vector<uint8_t>(128);
    
    // High ram. FF80-FFFE
    std::vector<uint8_t> hram = std::vector<uint8_t>(128);
    
    // Interrupt enable reg
    uint8_t int_enable;

  private:
    std::string romFname;

    // MBC
    bool ramEnable = false;
    uint8_t cartType;
    uint8_t mbcMode = ROM_MODE;

  public:
    Cpu * cpu;

    void Init();
    void Write(uint16_t address, uint8_t val);
    void Write_MMIO(uint16_t address, uint8_t val);
    void MBC(uint16_t address, uint8_t value);
    void SwitchBanks(uint8_t bankNum);
    uint8_t Read(uint16_t address) const;
    uint8_t CopyRom(std::string fname);
    uint8_t * GetAddressPointer(uint16_t address);

    uint8_t const BitTest(uint16_t address, uint8_t bit);
    void BitSet(uint16_t address, uint8_t bit);
    void BitClear(uint16_t address, uint8_t bit);
};

#endif
