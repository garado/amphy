
#include <stdio.h>
#include <iostream>
#include <array>
#include <fstream>
#include <vector>

class Cpu 
{
    public:
        // Internal registers
        uint8_t a = 0x01;
        uint8_t f = 0xB0;
        uint8_t b = 0x00;
        uint8_t c = 0x13;
        uint8_t d = 0x00;
        uint8_t e = 0xD8;
        uint8_t h = 0x01;
        uint8_t l = 0x4D;
        uint16_t sp = 0xFFFE; // stack pointer
        uint16_t pc = 0x0100; // program counter

        // Enums for flag register
        typedef enum Flags {
            CARRY = 0x4, HALF_CARRY, SUB, ZERO
        } Flags;

        // Memory map
        // Rom bank 0. Fixed. 0000 - 3FFF
        std::vector<uint8_t> rom_00;
        
        // Rom bank 1. Switchable. 4000 - 7FFF
        std::vector<uint8_t> rom_01;
        
        // Video ram. 8000-9FFF
        std::vector<uint8_t> vram;
        
        // From cart. Switchable. A000 - BFFF
        std::vector<uint8_t> ext_ram = std::vector<uint8_t>(8192);
        
        // C000 - CFFF
        std::vector<uint8_t> wram_0 = std::vector<uint8_t>(4096);
        
        // D000 - DFFF
        std::vector<uint8_t> wram_1 = std::vector<uint8_t>(4096);
        
        // Mirror of C000-DFFF. E000-FDFF
        std::vector<uint8_t> ech_ram = std::vector<uint8_t>(7680);
        
        // FE00-FE9F
        std::vector<uint8_t> oam;
        
        // IO registers FF00-FF7F
        std::vector<uint8_t> io_reg = std::vector<uint8_t>(128);
        
        // High ram. FF80-FFFE
        std::vector<uint8_t> hram = std::vector<uint8_t>(128);
        
        // Interrupt enable reg
        uint8_t int_enable;

        // Timing
        int cycleCount = 0; // in m-cycles, whatever the hell that is

    public:
        void copyRom(std::string fname);
        bool execute();
        uint8_t mem(uint16_t address);
        void mem(uint16_t address, uint8_t val);
        void regdump();
        void unknown(uint8_t opcode);

        uint8_t flag(Flags flag);
        void flag(Flags flag, bool val);
        

    public:
        void NOP();
        void DEC_n(uint8_t* reg);
        void XOR_n(uint8_t* reg);
        void LD_R_n(uint8_t* reg);
        void JR_cc_n(Flags flag, bool b);
        void LD_RR_nn(uint8_t* reg1, uint8_t* reg2);
        void LDD_RR_n(uint8_t* reg1, uint8_t* reg2);
        void LDH_n_R(uint8_t* reg);
        void LDH_n_A();
        void LDH_A_n();
        void JP_nn();
        void DI();
};