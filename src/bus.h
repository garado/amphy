
#include <stdio.h>
#include <iostream>
#include <vector>

#ifndef BUS_H
#define BUS_H

// Starting address for different regions of memory
#define ROM0_START    0x0000
#define ROM1_START    0x4000
#define VRAM_START    0x8000
#define EXTRAM_START  0xA000
#define WRAM0_START   0xC000
#define WRAM0_START   0xC000
#define WRAM1_START   0xD000
#define ECHRAM_START  0xE000
#define OAM_START     0xFE00
#define INVALID_START 0xFEA0
#define IO_START      0xFF00
#define HRAM_START    0xFF80

class Bus
{
  private:
    // Memory map
    // Rom bank 0. Fixed. 0000 - 3FFF
    std::vector<uint8_t> rom_00;
    
    // Rom bank 1. Switchable. 4000 - 7FFF
    std::vector<uint8_t> rom_01;
    
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

  public:
    bool allow_div = false;
    void init();
    uint8_t read(uint16_t address) const;
    void    write(uint16_t address, uint8_t val);
    uint8_t  CopyRom(std::string fname);
    uint8_t * GetAddressPointer(uint16_t address);
};

#endif
