
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
        uint16_t pc = 0x0100; // program counter

        uint16_t af() { return (a << 8) | f; }
        uint16_t bc() { return (b << 8) | c; }
        uint16_t de() { return (d << 8) | e; }
        uint16_t hl() { return (h << 8) | l; }

        // Enums for CPU flags
        typedef enum Flags {
            CARRY = 0x4, HALF_CARRY, SUB, ZERO
        } Flags;

        // Timing
        int cycleCount = 0; // in t-cycles

        bool cpuEnabled = true;
        uint8_t disableInterrupts = 0;
        uint8_t enableInterrupts = 0;
        bool interruptMasterEnable = true;

    public:
        bool execute();
        void regdump();
        void unknown(uint8_t opcode);
        int getCycles() { return cycleCount; }
        uint8_t getFlag(Flags flag);
        void setFlag(Flags flag);
        void resetFlag(Flags flag);
        void setHalfCarryAdd(uint8_t a, uint8_t b);
        void setHalfCarrySub(uint8_t a, uint8_t b);
        void setCarryAdd(uint8_t a, uint8_t b);
        void setCarrySub(uint8_t a, uint8_t b);
        void setAddFlags(uint8_t a, uint8_t b);
        void setSubFlags(uint8_t a, uint8_t b);
        void errloop() { while(1){}; }

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
        void SUB_R1(uint8_t* R1);
        void SUB_HL();
        void SUB_n();
        void SBC_A_R1(uint8_t* R1);
        void SBC_A_HL();
        void AND_R1(uint8_t* R1);   // maybe consider making
        void AND_HL();              // just 3 functions for
        void AND_n();               // logical ops?
        void OR_R1(uint8_t* R1);
        void OR_HL();
        void OR_n();
        void XOR_R1(uint8_t* R1);
        void XOR_HL();
        void XOR_n();
        void CP_R1(uint8_t* R1);
        void CP_HL();
        void CP_n();
        void INC_R1(uint8_t* R1);
        void INC_HL();
        void DEC_R1(uint8_t* R1);
        void DEC_HL();

        // Jumps
        void JP_nn();
        void JP_cc_nn(Flags flag, bool cond);
        void JP_HL();
        void JR_n();
        void JR_cc_n(Flags flag, bool cond);

        // Misc
        void NOP();
        void HALT(); // !!!
        void STOP(); // !!!
        void DI();
        void EI();

    public:
        Cpu(Bus* bus_) { bus = bus_; }
        ~Cpu();

    private:
        Bus* bus;
};

#endif
