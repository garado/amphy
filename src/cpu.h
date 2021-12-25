
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
        int getCycles() { return cycleCount; }
        uint8_t getFlag(Flags flag);
        void setFlag(Flags flag);
        void resetFlag(Flags flag);
        
        void checkHalfCarry(uint8_t a, uint8_t b);
        void checkCarry(uint8_t a, uint8_t b);

    private:
        // 8-bit loads
        void LD_R1_n(uint8_t* reg);
        void LD_R1_R2(uint8_t* reg1, uint8_t* reg2);
        void LD_R1_HL(uint8_t* reg1);
        void LD_HL_R1(uint8_t* reg1);
        void LD_HL_n();
        void LD_A_n();
        void LD_nn_A();
        void LD_A_C();
        void LD_C_A();
        void LDD_A_HL();
        void LDD_HL_A();
        void LDI_A_HL();
        void LDI_HL_A();
        void LDH_n_A();
        void LDH_A_n();

        // hm... might refactor??? 
        // kind of same as LD R1 (HL)
        void LD_A_BC();
        void LD_A_DE();
        void LD_A_nn(); // ???
        void LD_BC_A();
        void LD_DE_A();
        void LD_HL_A();

        // 16-bit loads
        void LD_R1R2_nn(uint8_t* R1, uint8_t* R2);
        void LD_SP_nn();
        void LD_SP_HL();
        void LDHL_SP_n();
        void LD_nn_SP();
        void PUSH_R1R2(uint8_t* R1, uint8_t* R2);
        void POP_R1R2(uint8_t* R1, uint8_t* R2);

        // 8-bit ALU
        void ADD_A_R1(uint8_t* R1);
        void ADD_A_HL();
        void ADD_A_n();
        void ADC_A_R1(uint8_t* R1);
        void ADC_A_HL();
        void ADC_A_n();
        void XOR_R1(uint8_t* R1);
        void XOR_HL();
        void XOR_n();

        // Jumps
        void JP_nn();

        void errloop() { while(1){}; }

        // OLD!!! AGAIN

        // void LD_RR_nn(uint8_t* reg1, uint8_t* reg2);
        // void LDD_RR_n(uint8_t* reg1, uint8_t* reg2);
        // void LDH_n_R(uint8_t* reg);
        // void LDH_n_A();
        // void LD_HL_n();

        // // Misc
        // void NOP();
        // void DI();

        // // 8-bit ALU
        // void DEC_n(uint8_t* reg);
        // void XOR_n(uint8_t* reg);
        // void CP_A_n();

        // // Jump
        // void JR_cc_n(Flags flag, bool b);
        // void JP_nn();

    public:
        Cpu(Bus* bus_) { bus = bus_; }
        ~Cpu();

    private:
        Bus* bus;
};

#endif
