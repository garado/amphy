
#include <stdio.h>
#include <iostream>
#include <array>
#include <fstream>
#include <vector>

class Cpu 
{
    public:
        // Internal registers
        uint16_t af = 0x01B0;
        uint16_t bc = 0x0013;
        uint16_t de = 0x00D8;
        uint16_t hl = 0x014D;
        uint16_t sp = 0xFFFE; // stack pointer
        uint16_t pc = 0x0100; // program counter

        // Enums for flag register
        enum flags {
            ZERO, SUB, HALF_CARRY, CARRY
        };

        // Memory map
        // Rom bank 0. Fixed. 0000 - 3FFF
        std::vector<uint8_t> rom_00;
        
        // Rom bank 1. Switchable. 4000 - 7FFF
        std::vector<uint8_t> rom_01;
        
        // Video ram. 8000-9FFF
        std::vector<uint8_t> vram;
        
        // From cart. Switchable. A000 - BFFF
        std::vector<uint8_t> ext_ram;
        
        // C000 - CFFF
        std::vector<uint8_t> wram_0 = std::vector<uint8_t>(4096);
        
        // D000 - DFFF
        std::vector<uint8_t> wram_1 = std::vector<uint8_t>(4096);
        
        // Mirror of C000-DFFF. E000-FDFF
        std::vector<uint8_t> ech_ram = std::vector<uint8_t>(7680);
        
        // FE00-FE9F
        std::vector<uint8_t> oam;
        
        // IO registers FF00-FF7F
        std::vector<uint8_t> io_reg;
        
        // High ram. FF80-FFFE
        std::vector<uint8_t> hram;
        
        // Interrupt enable reg
        uint8_t int_enable;

    public:
        void copyRom(std::string fname);
        bool execute();
        uint8_t mem(uint16_t address);
        void mem(uint16_t address, uint8_t val);

    public:
        void NOP();
        
        void JP_nn();
        void XOR_a();

        void LD_HL_nn();
        void LD_B_n();
        void LD_C_n();
        void LDD_HL_A();
};