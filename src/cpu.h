
#include <stdio.h>
#include <iostream>
#include <array>
#include <fstream>
#include <vector>

class Cpu 
{
    public:
        // registers
        uint16_t af = 0x01B0;
        uint16_t bc = 0x0013;
        uint16_t de = 0x00D8;
        uint16_t hl = 0x014D;
        uint16_t sp = 0xFFFE; // stack pointer
        uint16_t pc = 0x0100; // program counter

        // memory
        std::vector<uint8_t> mem;

        // enums for flag register
        enum flags {
            ZERO, SUB, HALF_CARRY, CARRY
        };

    public:
        void copyRom(std::string fname);
        bool execute();

    public:
        void NOP();
        
        void JP_nn();
        void XOR_a();

        void LD_HL_nn();
        void LD_B_n();
        void LD_C_n();
        void LDD_HL_A();
};