
#include <stdio.h>
#include <iostream>
#include <array>
#include <fstream>
#include <vector>

#include "../bus.h"

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

        // Enums for reg F flags
        typedef enum CpuFlags {
            CARRY = 0x4, HALF_CARRY, SUB, ZERO
        } CpuFlags;

        // Misc vars
        int cycleCount = 0; // in t-cycles
        bool cpuEnabled = true;
        uint8_t disableInterrupts = 0;
        uint8_t enableInterrupts = 0;
        bool ime = true;
        bool ime_prev = true;

        // Array of function pointers to opcodes.
//        using Opcode = uint8_t(*)();
        typedef uint8_t(Cpu::*Opcode)();
        Opcode opcode[256] = {
        //  +0          +1          +2          +3          +4           +5         +6          +7          +8           +9          +A          +B         +C          +D          +E          +F
            NOP,        LD_BC_u16,  LD_atBC_A,  INC_BC,     INC_B,       DEC_B,     LD_B_u8,    RLCA,       LD_atu16_SP, ADD_HL_BC,  LD_A_atBC,  DEC_BC,    INC_C,      DEC_C,      LD_C_u8,    RRCA,       // 00+
            STOP,       LD_DE_u16,  LD_atDE_A,  INC_DE,     INC_D,       DEC_D,     LD_D_u8,    RLA,        JR_i8,       ADD_HL_DE,  LD_A_atDE,  DEC_DE,    INC_E,      DEC_E,      LD_E_u8,    RRA,        // 10+
            JR_NZ_i8,   LD_HL_u16,  LDI_atHL_A, INC_HL,     INC_H,       DEC_H,     LD_H_u8,    DAA,        JR_Z_i8,     ADD_HL_HL,  LDI_A_atHL, DEC_HL,    INC_L,      DEC_L,      LD_L_u8,    CPL,        // 20+
            JR_NC_i8,   LD_SP_u16,  LDD_atHL_A, INC_SP,     INC_atHL,    DEC_atHL,  LD_atHL_u8, SCF,        JR_C_i8,     ADD_HL_SP,  LDD_A_atHL, DEC_SP,    INC_A,      DEC_A,      LD_A_u8,    CCF,        // 30+
            LD_B_B,     LD_B_C,     LD_B_D,     LD_B_E,     LD_B_H,      LD_B_L,    LD_B_atHL,  LD_B_A,     LD_C_B,      LD_C_C,     LD_C_D,     LD_C_E,    LD_C_H,     LD_C_L,     LD_C_atHL,  LD_C_A,     // 40+
            LD_D_B,     LD_D_C,     LD_D_D,     LD_D_E,     LD_D_H,      LD_D_L,    LD_D_atHL,  LD_D_A,     LD_E_B,      LD_E_C,     LD_E_D,     LD_E_E,    LD_E_H,     LD_E_L,     LD_E_atHL,  LD_E_A,     // 50+
            LD_H_B,     LD_H_C,     LD_H_D,     LD_H_E,     LD_H_H,      LD_H_L,    LD_H_atHL,  LD_H_A,     LD_L_B,      LD_L_C,     LD_L_D,     LD_L_E,    LD_L_H,     LD_L_L,     LD_L_atHL,  LD_L_A,     // 60+
            LD_atHL_B,  LD_atHL_C,  LD_atHL_D,  LD_atHL_E,  LD_atHL_H,   LD_atHL_L, HALT,       LD_atHL_A,  LD_A_B,      LD_A_C,     LD_A_D,     LD_A_E,    LD_A_H,     LD_A_L,     LD_A_atHL,  LD_A_A,     // 70+
            ADD_A_B,    ADD_A_C,    ADD_A_D,    ADD_A_E,    ADD_A_H,     ADD_A_L,   ADD_A_atHL, ADD_A_A,    ADC_A_B,     ADC_A_C,    ADC_A_D,    ADC_A_E,   ADC_A_H,    ADC_A_L,    ADC_A_atHL, ADC_A_A,    // 80+
            SUB_A_B,    SUB_A_C,    SUB_A_D,    SUB_A_E,    SUB_A_H,     SUB_A_L,   SUB_A_atHL, SUB_A_A,    SBC_A_B,     SBC_A_C,    SBC_A_D,    SBC_A_E,   SBC_A_H,    SBC_A_L,    SBC_A_atHL, SBC_A_A,    // 90+
            AND_A_B,    AND_A_C,    AND_A_D,    AND_A_E,    AND_A_H,     AND_A_L,   AND_A_atHL, AND_A_A,    XOR_A_B,     XOR_A_C,    XOR_A_D,    XOR_A_E,   XOR_A_H,    XOR_A_L,    XOR_A_atHL, XOR_A_A,    // A0+
            OR_A_B,     OR_A_C,     OR_A_D,     OR_A_E,     OR_A_H,      OR_A_L,    OR_A_atHL,  OR_A_A,     CP_A_B,      CP_A_C,     CP_A_D,     CP_A_E,    CP_A_H,     CP_A_L,     CP_A_atHL,  CP_A_A,     // B0+
            RET_NZ,     POP_BC,     JP_NZ_u16,  JP_u16,     CALL_NZ_u16, PUSH_BC,   ADD_A_u8,   RST_00h,    RET_Z,       RET,        JP_Z_u16,   NULL,      CALL_Z_u16, CALL_u16,   ADC_A_u8,   RST_08h,    // C0+
            RET_NC,     POP_DE,     JP_NC_u16,  NULL,       CALL_NC_u16, PUSH_DE,   SUB_A_u8,   RST_10h,    RET_C,       RETI,       JP_C_u16,   NULL,      CALL_C_u16, NULL,       SBC_A_u8,   RST_18h,    // D0+
            LDH_atu8_A, POP_HL,     LDH_atC_A,  NULL,       NULL,        PUSH_HL,   AND_A_u8,   RST_20h,    ADD_SP_i8,   JP_HL,      LD_atu16_A, NULL,      NULL,       NULL,       XOR_A_u8,   RST_28h,    // E0+
            LDH_A_atu8, POP_AF,     LDH_A_atC,  DI,         NULL,        PUSH_AF,   OR_A_u8,    RST_30h,    LD_HL_SP_i8, LD_SP_HL,   LD_A_atu16, EI,        NULL,       NULL,       CP_A_u8,    RST_38h     // F0+
        };

        // Opcode ext_opcode[256] = {
        //     // Unimplemented!
        // };

    private:
        // Rotate/shift
        uint8_t RLA();
        uint8_t RRA();
        uint8_t RLCA();
        uint8_t RRCA();
        
        // Bit ops
        uint8_t AND_A_A();
        uint8_t AND_A_B();
        uint8_t AND_A_C();
        uint8_t AND_A_D();
        uint8_t AND_A_E();
        uint8_t AND_A_H();
        uint8_t AND_A_L();
        uint8_t AND_A_atHL();
        uint8_t AND_A_u8();

        uint8_t OR_A_A();
        uint8_t OR_A_B();
        uint8_t OR_A_C();
        uint8_t OR_A_D();
        uint8_t OR_A_E();
        uint8_t OR_A_H();
        uint8_t OR_A_L();
        uint8_t OR_A_atHL();
        uint8_t OR_A_u8();

        uint8_t XOR_A_A();
        uint8_t XOR_A_B();
        uint8_t XOR_A_C();
        uint8_t XOR_A_D();
        uint8_t XOR_A_E();
        uint8_t XOR_A_H();
        uint8_t XOR_A_L();
        uint8_t XOR_A_atHL();
        uint8_t XOR_A_u8();

        // 8-bit ALU
        uint8_t INC_A();
        uint8_t INC_B();
        uint8_t INC_C();
        uint8_t INC_D();
        uint8_t INC_E();
        uint8_t INC_H();
        uint8_t INC_L();
        uint8_t INC_atHL();

        uint8_t DEC_A();
        uint8_t DEC_B();
        uint8_t DEC_C();
        uint8_t DEC_D();
        uint8_t DEC_E();
        uint8_t DEC_H();
        uint8_t DEC_L();
        uint8_t DEC_atHL();

        uint8_t ADD_A_A();
        uint8_t ADD_A_B();
        uint8_t ADD_A_C();
        uint8_t ADD_A_D();
        uint8_t ADD_A_E();
        uint8_t ADD_A_H();
        uint8_t ADD_A_L();
        uint8_t ADD_A_atHL();
        uint8_t ADD_A_u8();

        uint8_t SUB_A_A();
        uint8_t SUB_A_B();
        uint8_t SUB_A_C();
        uint8_t SUB_A_D();
        uint8_t SUB_A_E();
        uint8_t SUB_A_H();
        uint8_t SUB_A_L();
        uint8_t SUB_A_atHL();
        uint8_t SUB_A_u8();

        uint8_t SBC_A_A();
        uint8_t SBC_A_B();
        uint8_t SBC_A_C();
        uint8_t SBC_A_D();
        uint8_t SBC_A_E();
        uint8_t SBC_A_H();
        uint8_t SBC_A_L();
        uint8_t SBC_A_atHL();
        uint8_t SBC_A_u8();

        uint8_t ADC_A_A();
        uint8_t ADC_A_B();
        uint8_t ADC_A_C();
        uint8_t ADC_A_D();
        uint8_t ADC_A_E();
        uint8_t ADC_A_H();
        uint8_t ADC_A_L();
        uint8_t ADC_A_atHL();
        uint8_t ADC_A_u8();

        uint8_t CP_A_A();
        uint8_t CP_A_B();
        uint8_t CP_A_C();
        uint8_t CP_A_D();
        uint8_t CP_A_E();
        uint8_t CP_A_H();
        uint8_t CP_A_L();
        uint8_t CP_A_atHL();
        uint8_t CP_A_u8();

        // 16-bit ALU
        uint8_t DEC_BC();
        uint8_t DEC_DE();
        uint8_t DEC_HL();
        uint8_t DEC_SP();
        uint8_t INC_BC();
        uint8_t INC_DE();
        uint8_t INC_HL();
        uint8_t INC_SP();
        uint8_t ADD_HL_BC();
        uint8_t ADD_HL_DE();
        uint8_t ADD_HL_HL();
        uint8_t ADD_HL_SP();
        uint8_t ADD_SP_i8();

        // 8-bit loads
        uint8_t LD_A_atHL();
        uint8_t LD_B_atHL();
        uint8_t LD_C_atHL();
        uint8_t LD_D_atHL();
        uint8_t LD_E_atHL();
        uint8_t LD_H_atHL();
        uint8_t LD_L_atHL();

        uint8_t LD_atHL_A();
        uint8_t LD_atHL_B();
        uint8_t LD_atHL_C();
        uint8_t LD_atHL_D();
        uint8_t LD_atHL_E();
        uint8_t LD_atHL_H();
        uint8_t LD_atHL_L();
        uint8_t LD_atHL_u8();

        uint8_t LD_A_u8();
        uint8_t LD_B_u8();
        uint8_t LD_C_u8();
        uint8_t LD_D_u8();
        uint8_t LD_E_u8();
        uint8_t LD_H_u8();
        uint8_t LD_L_u8();

        uint8_t LD_A_A();   // Register to register
        uint8_t LD_A_B();
        uint8_t LD_A_C();
        uint8_t LD_A_D();
        uint8_t LD_A_E();
        uint8_t LD_A_H();
        uint8_t LD_A_L();
        uint8_t LD_B_A();
        uint8_t LD_B_B();
        uint8_t LD_B_C();
        uint8_t LD_B_D();
        uint8_t LD_B_E();
        uint8_t LD_B_H();
        uint8_t LD_B_L();
        uint8_t LD_C_A();
        uint8_t LD_C_B();
        uint8_t LD_C_C();
        uint8_t LD_C_D();
        uint8_t LD_C_E();
        uint8_t LD_C_H();
        uint8_t LD_C_L();
        uint8_t LD_D_A();
        uint8_t LD_D_B();
        uint8_t LD_D_C();
        uint8_t LD_D_D();
        uint8_t LD_D_E();
        uint8_t LD_D_H();
        uint8_t LD_D_L();
        uint8_t LD_E_A();
        uint8_t LD_E_B();
        uint8_t LD_E_C();
        uint8_t LD_E_D();
        uint8_t LD_E_E();
        uint8_t LD_E_H();
        uint8_t LD_E_L();
        uint8_t LD_H_A();
        uint8_t LD_H_B();
        uint8_t LD_H_C();
        uint8_t LD_H_D();
        uint8_t LD_H_E();
        uint8_t LD_H_H();
        uint8_t LD_H_L();
        uint8_t LD_L_A();
        uint8_t LD_L_B();
        uint8_t LD_L_C();
        uint8_t LD_L_D();
        uint8_t LD_L_E();
        uint8_t LD_L_H();
        uint8_t LD_L_L();

        uint8_t LD_BC_A();
        uint8_t LD_atBC_A();
        uint8_t LD_atDE_A();
        uint8_t LD_A_atBC();
        uint8_t LD_A_atDE();
        uint8_t LD_B_u8();

        uint8_t LD_atu16_A();
        uint8_t LD_A_atu16();

        uint8_t LDH_atu8_A();
        uint8_t LDH_atC_A();
        uint8_t LDH_A_atu8();
        uint8_t LDH_A_atC();

        uint8_t LDD_A_atHL();
        uint8_t LDD_atHL_A();
        uint8_t LDI_A_atHL();
        uint8_t LDI_atHL_A();

        // 16-bit loads
        uint8_t LD_BC_u16();
        uint8_t LD_DE_u16();
        uint8_t LD_HL_u16();
        uint8_t LD_SP_u16();
        uint8_t LD_atu16_SP();
        uint8_t LD_SP_HL();
        uint8_t LD_HL_SP_i8();
        uint8_t PUSH_AF();
        uint8_t PUSH_BC();
        uint8_t PUSH_DE();
        uint8_t PUSH_HL();
        uint8_t POP_AF();
        uint8_t POP_BC();
        uint8_t POP_DE();
        uint8_t POP_HL();

        // Jumps
        uint8_t JP_HL();
        uint8_t JR_i8();
        uint8_t JP_u16();
        uint8_t JP_NZ_u16();
        uint8_t JP_Z_u16();
        uint8_t JP_NC_u16();
        uint8_t JP_C_u16();
        uint8_t JR_NZ_i8();
        uint8_t JR_Z_i8();
        uint8_t JR_NC_i8();
        uint8_t JR_C_i8();

        // Returns
        uint8_t RET();
        uint8_t RETI();

        // Restart
        uint8_t RST_00h();
        uint8_t RST_08h();
        uint8_t RST_10h();
        uint8_t RST_18h();
        uint8_t RST_20h();
        uint8_t RST_28h();
        uint8_t RST_30h();
        uint8_t RST_38h();

        // Misc
        uint8_t CPL();
        uint8_t CCF();
        uint8_t SCF();
        uint8_t NOP();
        uint8_t HALT(); // !!!
        uint8_t STOP(); // !!!
        uint8_t DI();
        uint8_t EI();

    public:
        bool execute();
        void regdump();
        bool getFlag(CpuFlags flag);
        void assignFlag(CpuFlags flag, bool val);
        void unknown(uint8_t opcode);
        void addCycles(uint8_t cycles) { cycleCount += cycles; }
        uint8_t getCycles() { return cycleCount; }
        void setHalfCarryAdd(uint8_t a, uint8_t b);
        void setHalfCarrySub(uint8_t a, uint8_t b);
        void setCarryAdd(uint8_t a, uint8_t b);
        void setCarrySub(uint8_t a, uint8_t b);
        void setAddFlags(uint8_t a, uint8_t b);
        void setSubFlags(uint8_t a, uint8_t b);

        void setHalfCarryAdd(uint16_t a, uint16_t b);
        void setHalfCarrySub(uint16_t a, uint16_t b);
        void setCarryAdd(uint16_t a, uint16_t b);
        void setCarrySub(uint16_t a, uint16_t b);
        void setAddFlags(uint16_t a, uint16_t b);
        void setSubFlags(uint16_t a, uint16_t b);

    public:
        Cpu(Bus* bus_) { bus = bus_; }
        ~Cpu();

    private:
        Bus* bus;
};

#endif
