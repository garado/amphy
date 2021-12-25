
#include <stdio.h>
#include <iostream>
#include <array>
#include <fstream>
#include <vector>

#include "bus.h"

#ifndef CPU_H
#define CPU_H

class Cpu 
{
    private:
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
        uint16_t pc = 0x0000; // program counter

        // Enums for flag register
        typedef enum Flags {
            CARRY = 0x4, HALF_CARRY, SUB, ZERO
        } Flags;

        // Timing
        int cycleCount = 0; // in t-cycles

    public:
        bool execute();
        void regdump();
        void unknown(uint8_t opcode);
        uint8_t flag(Flags flag);
        void flag(Flags flag, bool val);
        int getCycles() { return cycleCount; }

    public:
        // Misc
        void NOP();
        void DI();

        // 8-bit ALU
        void DEC_n(uint8_t* reg);
        void XOR_n(uint8_t* reg);
        void CP_A_n();

        // Jump
        void JR_cc_n(Flags flag, bool b);
        void JP_nn();

        // Loads
        void LD_R_n(uint8_t* reg);
        void LD_RR_nn(uint8_t* reg1, uint8_t* reg2);
        void LDD_RR_n(uint8_t* reg1, uint8_t* reg2);
        void LDH_n_R(uint8_t* reg);
        void LDH_n_A();
        void LDH_A_n();

    public:
        Cpu(Bus* bus_) { bus = bus_; }
        ~Cpu();

    private:
        Bus* bus;
};

#endif
