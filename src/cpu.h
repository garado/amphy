
#include <stdio.h>
#include <iostream>
#include <array>
#include <fstream>
#include <vector>

// Memory locations

class Cpu 
{
    public:
        // registers
        uint8_t a;
        uint8_t b;
        uint8_t c;
        uint8_t d;
        uint8_t e;
        uint8_t f;
        uint8_t h;
        uint16_t sp = 0xFFFE; // stack pointer
        uint16_t pc = 0x0100; // program counter
    
        // memory
//        uint8_t mem[0xFFFF];
        uint8_t what;
        std::vector<uint8_t> mem;

    public:
        void copyRom(std::string fname);
        void execute(uint8_t opcode);
};