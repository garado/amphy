
#include <stdio.h>
#include <iostream>
#include <vector>

#ifndef BUS_H
#define BUS_H

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
    std::vector<uint8_t> oam = std::vector<uint8_t>(160);
    
    // IO registers FF00-FF7F
    std::vector<uint8_t> io_reg = std::vector<uint8_t>(128);
    
    // High ram. FF80-FFFE
    std::vector<uint8_t> hram = std::vector<uint8_t>(128);
    
    // Interrupt enable reg
    uint8_t int_enable;

  public:
    uint8_t read(uint16_t address) const;
    void    write(uint16_t address, uint8_t val);
    int8_t  CopyRom(std::string fname);
};

#endif
