
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
    uint8_t a = 0x11; // blargg test rom init
    uint8_t f = 0x80;
    uint8_t b = 0x00;
    uint8_t c = 0x00;
    uint8_t d = 0xFF;
    uint8_t e = 0x56;
    uint8_t h = 0x00;
    uint8_t l = 0x0D;

    // uint8_t a = 0x01;
    // uint8_t f = 0xB0;
    // uint8_t b = 0x00;
    // uint8_t c = 0x13;
    // uint8_t d = 0x00;
    // uint8_t e = 0xD8;
    // uint8_t h = 0x01;
    // uint8_t l = 0x4D;

    uint8_t op = 0;     // opcode
    uint16_t sp = 0xFFFE; // stack pointer
    uint16_t pc = 0x0100; // program counter

    // Enums for reg F flags
    typedef enum CpuFlags {
      CARRY = 0x4,  // bit 4
      HALF_CARRY,   // bit 5
      SUB,  // bit 6
      ZERO  // bit 7
    } CpuFlags;

    // Misc vars
    int   cycleCount = 0; // in t-cycles
    bool  cpuEnabled = true;
    uint8_t disableInterrupts = 0;
    uint8_t enableInterrupts = 0;
    bool ime = true;
    bool ime_prev = true;

  public:
    // Array of function pointers to opcodes.
    typedef uint8_t (Cpu::*Opcode)();
    Opcode opcodes[256] = { 
    //  +0              +1                +2                +3                +4                  +5                +6                +7                +8                  +9                +A                +B              +C                +D              +E                +F
      &Cpu::NOP,        &Cpu::LD_BC_u16,  &Cpu::LD_atBC_A,  &Cpu::INC_BC,     &Cpu::INC_B,        &Cpu::DEC_B,      &Cpu::LD_B_u8,    &Cpu::RLCA,       &Cpu::LD_atu16_SP,  &Cpu::ADD_HL_BC,  &Cpu::LD_A_atBC,  &Cpu::DEC_BC,   &Cpu::INC_C,      &Cpu::DEC_C,    &Cpu::LD_C_u8,    &Cpu::RRCA,     // 00+
      &Cpu::STOP,       &Cpu::LD_DE_u16,  &Cpu::LD_atDE_A,  &Cpu::INC_DE,     &Cpu::INC_D,        &Cpu::DEC_D,      &Cpu::LD_D_u8,    &Cpu::RLA,        &Cpu::JR_i8,        &Cpu::ADD_HL_DE,  &Cpu::LD_A_atDE,  &Cpu::DEC_DE,   &Cpu::INC_E,      &Cpu::DEC_E,    &Cpu::LD_E_u8,    &Cpu::RRA,      // 10+
      &Cpu::JR_NZ_i8,   &Cpu::LD_HL_u16,  &Cpu::LDI_atHL_A, &Cpu::INC_HL,     &Cpu::INC_H,        &Cpu::DEC_H,      &Cpu::LD_H_u8,    &Cpu::DAA,        &Cpu::JR_Z_i8,      &Cpu::ADD_HL_HL,  &Cpu::LDI_A_atHL, &Cpu::DEC_HL,   &Cpu::INC_L,      &Cpu::DEC_L,    &Cpu::LD_L_u8,    &Cpu::CPL,      // 20+
      &Cpu::JR_NC_i8,   &Cpu::LD_SP_u16,  &Cpu::LDD_atHL_A, &Cpu::INC_SP,     &Cpu::INC_atHL,     &Cpu::DEC_atHL,   &Cpu::LD_atHL_u8, &Cpu::SCF,        &Cpu::JR_C_i8,      &Cpu::ADD_HL_SP,  &Cpu::LDD_A_atHL, &Cpu::DEC_SP,   &Cpu::INC_A,      &Cpu::DEC_A,    &Cpu::LD_A_u8,    &Cpu::CCF,      // 30+
      &Cpu::LD_B_B,     &Cpu::LD_B_C,     &Cpu::LD_B_D,     &Cpu::LD_B_E,     &Cpu::LD_B_H,       &Cpu::LD_B_L,     &Cpu::LD_B_atHL,  &Cpu::LD_B_A,     &Cpu::LD_C_B,       &Cpu::LD_C_C,     &Cpu::LD_C_D,     &Cpu::LD_C_E,   &Cpu::LD_C_H,     &Cpu::LD_C_L,   &Cpu::LD_C_atHL,  &Cpu::LD_C_A,   // 40+
      &Cpu::LD_D_B,     &Cpu::LD_D_C,     &Cpu::LD_D_D,     &Cpu::LD_D_E,     &Cpu::LD_D_H,       &Cpu::LD_D_L,     &Cpu::LD_D_atHL,  &Cpu::LD_D_A,     &Cpu::LD_E_B,       &Cpu::LD_E_C,     &Cpu::LD_E_D,     &Cpu::LD_E_E,   &Cpu::LD_E_H,     &Cpu::LD_E_L,   &Cpu::LD_E_atHL,  &Cpu::LD_E_A,   // 50+
      &Cpu::LD_H_B,     &Cpu::LD_H_C,     &Cpu::LD_H_D,     &Cpu::LD_H_E,     &Cpu::LD_H_H,       &Cpu::LD_H_L,     &Cpu::LD_H_atHL,  &Cpu::LD_H_A,     &Cpu::LD_L_B,       &Cpu::LD_L_C,     &Cpu::LD_L_D,     &Cpu::LD_L_E,   &Cpu::LD_L_H,     &Cpu::LD_L_L,   &Cpu::LD_L_atHL,  &Cpu::LD_L_A,   // 60+
      &Cpu::LD_atHL_B,  &Cpu::LD_atHL_C,  &Cpu::LD_atHL_D,  &Cpu::LD_atHL_E,  &Cpu::LD_atHL_H,    &Cpu::LD_atHL_L,  &Cpu::HALT,       &Cpu::LD_atHL_A,  &Cpu::LD_A_B,       &Cpu::LD_A_C,     &Cpu::LD_A_D,     &Cpu::LD_A_E,   &Cpu::LD_A_H,     &Cpu::LD_A_L,   &Cpu::LD_A_atHL,  &Cpu::LD_A_A,   // 70+
      &Cpu::ADD_A_B,    &Cpu::ADD_A_C,    &Cpu::ADD_A_D,    &Cpu::ADD_A_E,    &Cpu::ADD_A_H,      &Cpu::ADD_A_L,    &Cpu::ADD_A_atHL, &Cpu::ADD_A_A,    &Cpu::ADC_A_B,      &Cpu::ADC_A_C,    &Cpu::ADC_A_D,    &Cpu::ADC_A_E,  &Cpu::ADC_A_H,    &Cpu::ADC_A_L,  &Cpu::ADC_A_atHL, &Cpu::ADC_A_A,  // 80+
      &Cpu::SUB_A_B,    &Cpu::SUB_A_C,    &Cpu::SUB_A_D,    &Cpu::SUB_A_E,    &Cpu::SUB_A_H,      &Cpu::SUB_A_L,    &Cpu::SUB_A_atHL, &Cpu::SUB_A_A,    &Cpu::SBC_A_B,      &Cpu::SBC_A_C,    &Cpu::SBC_A_D,    &Cpu::SBC_A_E,  &Cpu::SBC_A_H,    &Cpu::SBC_A_L,  &Cpu::SBC_A_atHL, &Cpu::SBC_A_A,  // 90+
      &Cpu::AND_A_B,    &Cpu::AND_A_C,    &Cpu::AND_A_D,    &Cpu::AND_A_E,    &Cpu::AND_A_H,      &Cpu::AND_A_L,    &Cpu::AND_A_atHL, &Cpu::AND_A_A,    &Cpu::XOR_A_B,      &Cpu::XOR_A_C,    &Cpu::XOR_A_D,    &Cpu::XOR_A_E,  &Cpu::XOR_A_H,    &Cpu::XOR_A_L,  &Cpu::XOR_A_atHL, &Cpu::XOR_A_A,  // A0+
      &Cpu::OR_A_B,     &Cpu::OR_A_C,     &Cpu::OR_A_D,     &Cpu::OR_A_E,     &Cpu::OR_A_H,       &Cpu::OR_A_L,     &Cpu::OR_A_atHL,  &Cpu::OR_A_A,     &Cpu::CP_A_B,       &Cpu::CP_A_C,     &Cpu::CP_A_D,     &Cpu::CP_A_E,   &Cpu::CP_A_H,     &Cpu::CP_A_L,   &Cpu::CP_A_atHL,  &Cpu::CP_A_A,   // B0+
      &Cpu::RET_NZ,     &Cpu::POP_BC,     &Cpu::JP_NZ_u16,  &Cpu::JP_u16,     &Cpu::CALL_NZ_u16,  &Cpu::PUSH_BC,    &Cpu::ADD_A_u8,   &Cpu::RST_00h,    &Cpu::RET_Z,        &Cpu::RET,        &Cpu::JP_Z_u16,   NULL,           &Cpu::CALL_Z_u16, &Cpu::CALL_u16, &Cpu::ADC_A_u8,   &Cpu::RST_08h,  // C0+
      &Cpu::RET_NC,     &Cpu::POP_DE,     &Cpu::JP_NC_u16,  NULL,             &Cpu::CALL_NC_u16,  &Cpu::PUSH_DE,    &Cpu::SUB_A_u8,   &Cpu::RST_10h,    &Cpu::RET_C,        &Cpu::RETI,       &Cpu::JP_C_u16,   NULL,           &Cpu::CALL_C_u16, NULL,           &Cpu::SBC_A_u8,   &Cpu::RST_18h,  // D0+
      &Cpu::LDH_atu8_A, &Cpu::POP_HL,     &Cpu::LDH_atC_A,  NULL,             NULL,               &Cpu::PUSH_HL,    &Cpu::AND_A_u8,   &Cpu::RST_20h,    &Cpu::ADD_SP_i8,    &Cpu::JP_HL,      &Cpu::LD_atu16_A, NULL,           NULL,             NULL,           &Cpu::XOR_A_u8,   &Cpu::RST_28h,  // E0+
      &Cpu::LDH_A_atu8, &Cpu::POP_AF,     &Cpu::LDH_A_atC,  &Cpu::DI,         NULL,               &Cpu::PUSH_AF,    &Cpu::OR_A_u8,    &Cpu::RST_30h,    &Cpu::LD_HL_SP_i8,  &Cpu::LD_SP_HL,   &Cpu::LD_A_atu16, &Cpu::EI,       NULL,             NULL,           &Cpu::CP_A_u8,    &Cpu::RST_38h,  // F0+
    };

    const char* opcode_names[256] = { 
      // +0         +1            +2            +3            +4              +5            +6            +7            +8              +9            +A            +B          +C            +D          +E            +F
      "NOP",        "LD_BC_u16",  "LD_atBC_A",  "INC_BC",     "INC_B",        "DEC_B",      "LD_B_u8",    "RLCA",       "LD_atu16_SP",  "ADD_HL_BC",  "LD_A_atBC",  "DEC_BC",   "INC_C",      "DEC_C",    "LD_C_u8",    "RRCA",     // 00+
      "STOP",       "LD_DE_u16",  "LD_atDE_A",  "INC_DE",     "INC_D",        "DEC_D",      "LD_D_u8",    "RLA",        "JR_i8",        "ADD_HL_DE",  "LD_A_atDE",  "DEC_DE",   "INC_E",      "DEC_E",    "LD_E_u8",    "RRA",      // 10+
      "JR_NZ_i8",   "LD_HL_u16",  "LDI_atHL_A", "INC_HL",     "INC_H",        "DEC_H",      "LD_H_u8",    "DAA",        "JR_Z_i8",      "ADD_HL_HL",  "LDI_A_atHL", "DEC_HL",   "INC_L",      "DEC_L",    "LD_L_u8",    "CPL",      // 20+
      "JR_NC_i8",   "LD_SP_u16",  "LDD_atHL_A", "INC_SP",     "INC_atHL",     "DEC_atHL",   "LD_atHL_u8", "SCF",        "JR_C_i8",      "ADD_HL_SP",  "LDD_A_atHL", "DEC_SP",   "INC_A",      "DEC_A",    "LD_A_u8",    "CCF",      // 30+
      "LD_B_B",     "LD_B_C",     "LD_B_D",     "LD_B_E",     "LD_B_H",       "LD_B_L",     "LD_B_atHL",  "LD_B_A",     "LD_C_B",       "LD_C_C",     "LD_C_D",     "LD_C_E",   "LD_C_H",     "LD_C_L",   "LD_C_atHL",  "LD_C_A",   // 40+
      "LD_D_B",     "LD_D_C",     "LD_D_D",     "LD_D_E",     "LD_D_H",       "LD_D_L",     "LD_D_atHL",  "LD_D_A",     "LD_E_B",       "LD_E_C",     "LD_E_D",     "LD_E_E",   "LD_E_H",     "LD_E_L",   "LD_E_atHL",  "LD_E_A",   // 50+
      "LD_H_B",     "LD_H_C",     "LD_H_D",     "LD_H_E",     "LD_H_H",       "LD_H_L",     "LD_H_atHL",  "LD_H_A",     "LD_L_B",       "LD_L_C",     "LD_L_D",     "LD_L_E",   "LD_L_H",     "LD_L_L",   "LD_L_atHL",  "LD_L_A",   // 60+
      "LD_atHL_B",  "LD_atHL_C",  "LD_atHL_D",  "LD_atHL_E",  "LD_atHL_H",    "LD_atHL_L",  "HALT",       "LD_atHL_A",  "LD_A_B",       "LD_A_C",     "LD_A_D",     "LD_A_E",   "LD_A_H",     "LD_A_L",   "LD_A_atHL",  "LD_A_A",   // 70+
      "ADD_A_B",    "ADD_A_C",    "ADD_A_D",    "ADD_A_E",    "ADD_A_H",      "ADD_A_L",    "ADD_A_atHL", "ADD_A_A",    "ADC_A_B",      "ADC_A_C",    "ADC_A_D",    "ADC_A_E",  "ADC_A_H",    "ADC_A_L",  "ADC_A_atHL", "ADC_A_A",  // 80+
      "SUB_A_B",    "SUB_A_C",    "SUB_A_D",    "SUB_A_E",    "SUB_A_H",      "SUB_A_L",    "SUB_A_atHL", "SUB_A_A",    "SBC_A_B",      "SBC_A_C",    "SBC_A_D",    "SBC_A_E",  "SBC_A_H",    "SBC_A_L",  "SBC_A_atHL", "SBC_A_A",  // 90+
      "AND_A_B",    "AND_A_C",    "AND_A_D",    "AND_A_E",    "AND_A_H",      "AND_A_L",    "AND_A_atHL", "AND_A_A",    "XOR_A_B",      "XOR_A_C",    "XOR_A_D",    "XOR_A_E",  "XOR_A_H",    "XOR_A_L",  "XOR_A_atHL", "XOR_A_A",  // A0+
      "OR_A_B",     "OR_A_C",     "OR_A_D",     "OR_A_E",     "OR_A_H",       "OR_A_L",     "OR_A_atHL",  "OR_A_A",     "CP_A_B",       "CP_A_C",     "CP_A_D",     "CP_A_E",   "CP_A_H",     "CP_A_L",   "CP_A_atHL",  "CP_A_A",   // B0+
      "RET_NZ",     "POP_BC",     "JP_NZ_u16",  "JP_u16",     "CALL_NZ_u16",  "PUSH_BC",    "ADD_A_u8",   "RST_00h",    "RET_Z",        "RET",        "JP_Z_u16",   "NULL",     "CALL_Z_u16", "CALL_u16", "ADC_A_u8",   "RST_08h",  // C0+
      "RET_NC",     "POP_DE",     "JP_NC_u16",  "NULL",       "CALL_NC_u16",  "PUSH_DE",    "SUB_A_u8",   "RST_10h",    "RET_C",        "RETI",       "JP_C_u16",   "NULL",     "CALL_C_u16", "NULL",     "SBC_A_u8",   "RST_18h",  // D0+
      "LDH_atu8_A", "POP_HL",     "LDH_atC_A",  "NULL",       "NULL",         "PUSH_HL",    "AND_A_u8",   "RST_20h",    "ADD_SP_i8",    "JP_HL",      "LD_atu16_A", "NULL",     "NULL",       "NULL",     "XOR_A_u8",   "RST_28h",  // E0+
      "LDH_A_atu8", "POP_AF",     "LDH_A_atC",  "DI",         "NULL",         "PUSH_AF",    "OR_A_u8",    "RST_30h",    "LD_HL_SP_i8",  "LD_SP_HL",   "LD_A_atu16", "EI",       "NULL",       "NULL",     "CP_A_u8",    "RST_38h",  // F0+
    };

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

    // Calls
    uint8_t CALL_NZ_u16();
    uint8_t CALL_Z_u16();
    uint8_t CALL_u16();
    uint8_t CALL_NC_u16();
    uint8_t CALL_C_u16();

    // Returns
    uint8_t RET_NZ();
    uint8_t RET_Z();
    uint8_t RET();
    uint8_t RETI();
    uint8_t RET_NC();
    uint8_t RET_C();

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
    uint8_t DAA();

    // Misc helper functions for common operations within opcodes
    void SetHalfCarryAdd(uint8_t a, uint8_t b);
    void SetHalfCarrySub(uint8_t a, uint8_t b);
    void SetCarryAdd(uint8_t a, uint8_t b);
    void SetCarrySub(uint8_t a, uint8_t b);
    void SetAddFlags(uint8_t a, uint8_t b);
    void SetSubFlags(uint8_t a, uint8_t b);

    void SetHalfCarryAdd(uint16_t a, uint16_t b);
    void SetHalfCarrySub(uint16_t a, uint16_t b);
    void SetCarryAdd(uint16_t a, uint16_t b);
    void SetCarrySub(uint16_t a, uint16_t b);
    void SetAddFlags(uint16_t a, uint16_t b);
    void SetSubFlags(uint16_t a, uint16_t b);

    uint8_t LD_n_atHL(uint8_t * reg);
    uint8_t LD_atHL_n(uint8_t * reg);
    uint8_t LD_n_u8(uint8_t * reg);
    uint8_t LD_n_m(uint8_t * src, uint8_t * dst);
    uint8_t LD_n_atM(uint8_t * src, uint16_t address);
    uint8_t Push_NM(uint16_t regpair);
    uint8_t Pop_NM(uint8_t * upper, uint8_t * lower);

    uint8_t INC_n(uint8_t * reg);
    uint8_t DEC_n(uint8_t * reg);
    uint8_t ADD_A_n(uint8_t * reg);
    uint8_t SUB_A_n(uint8_t * reg);
    uint8_t SBC_A_n(uint8_t * reg);
    uint8_t ADC_A_n(uint8_t * reg);
    uint8_t CP_A_n(uint8_t *reg);
    uint8_t DEC_nm(uint8_t * upper_reg, uint8_t * lower_reg);
    uint8_t INC_nm(uint8_t * upper_reg, uint8_t * lower_reg);
    uint8_t ADD_HL_nm(uint8_t * upper, uint8_t * lower);
    uint8_t JP_flag_u16(CpuFlags flag, uint8_t val);
    uint8_t CALL_flag_u16(CpuFlags flag, uint8_t val);
    uint8_t JR_flag_i8(CpuFlags flag, uint8_t val);
    uint8_t AND_A_n(uint8_t * reg);
    uint8_t OR_A_n(uint8_t * reg);
    uint8_t XOR_A_n(uint8_t * reg);

    uint8_t   Pop8Bit(void);
    uint16_t  Pop16Bit(void);
    void Push8Bit(uint8_t val);
    void Push16Bit(uint16_t val);

    uint8_t   FetchNextByte(void);
    uint16_t  FetchNextTwoBytes(void);

  public:
    uint8_t execute();
    bool GetFlag(CpuFlags flag);
    void AssignFlag(CpuFlags flag, bool val);
    void unknown(uint8_t opcode);
    void AddCycles(uint8_t cycles) { cycleCount += cycles; }
    uint8_t GetCycles() { return cycleCount; }

    // Register helper functions
    void af(uint16_t af);
    void bc(uint16_t bc);
    void de(uint16_t de);
    void hl(uint16_t hl);
    
    uint16_t af() const { return (a << 8) | f; }
    uint16_t bc() const { return (b << 8) | c; }
    uint16_t de() const { return (d << 8) | e; }
    uint16_t hl() const { return (h << 8) | l; }

    uint16_t getSp() const { return sp; }
    uint16_t getPc() const { return pc; }
    uint8_t getOp() const { return op; }

  public:
    Cpu(Bus* bus_) {
      bus = bus_; 
    }

  private:
    Bus* bus;
};

#endif
