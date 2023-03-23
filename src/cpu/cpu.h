
#include <stdio.h>
#include <iostream>
#include <array>
#include <fstream>
#include <vector>

#include "../bus.h"
#include "../utils/debug.h"

#ifndef CPU_H
#define CPU_H

// Defines for interrupts
#define ISR_ADDR_VBLANK 0x40
#define INT_VBLANK_BIT  0x0 // Bit 0
#define INT_VBLANK_MASK(x) ((unsigned) x & 0x1)
#define INT_VBLANK_CLEAR(x) ((unsigned) x & ~(0x1 << INT_VBLANK_BIT ))

#define ISR_ADDR_STAT   0x48
#define INT_STAT_BIT    0x1 // Bit 1
#define INT_STAT_MASK(x) ((unsigned) (x >> INT_STAT_BIT) & 0x1)
#define INT_STAT_CLEAR(x) ((unsigned) x & ~(0x1 << INT_STAT_BIT ))

#define ISR_ADDR_TIMER  0x50
#define INT_TIMER_BIT   0x2
#define INT_TIMER_MASK(x) (((unsigned) x >> INT_TIMER_BIT) & 0x1)
#define INT_TIMER_CLEAR(x) ((unsigned) x & ~(0x1 << INT_TIMER_BIT ))

#define ISR_ADDR_SERIAL 0x58
#define INT_SERIAL_BIT  0x3
#define INT_SERIAL_MASK(x) ((unsigned) (x >> INT_SERIAL_BIT) & 0x1)
#define INT_SERIAL_CLEAR(x) ((unsigned) x & ~(0x1 << INT_SERIAL_BIT ))

#define ISR_ADDR_JOYPAD 0x60
#define INT_JOYPAD_BIT  0x4
#define INT_JOYPAD_MASK(x) ((unsigned) (x >> INT_JOYPAD_BIT) & 0x1)
#define INT_JOYPAD_CLEAR(x) ((unsigned) x & ~(0x1 << INT_JOYPAD_BIT ))

#define EI_COUNTER_INIT 2

// Timer control
#define TAC_ENABLE_BIT 0x2
#define TAC_ENABLE_MASK(x) (((unsigned) x >> TAC_ENABLE_BIT) & 0x1)
#define TAC_SELECT_MASK(x) (((unsigned) x & 0b11))
#define TAC_1024 0b00
#define TAC_16   0b01
#define TAC_64   0b10
#define TAC_256  0b11

class Cpu 
{
  private:
    // Internal registers
    uint16_t sysclk = 0x1EF0;

    uint8_t a = 0x01; // gbdoc init
    uint8_t f = 0xB0;
    uint8_t b = 0x00;
    uint8_t c = 0x13;
    uint8_t d = 0x00;
    uint8_t e = 0xD8;
    uint8_t h = 0x01;
    uint8_t l = 0x4D;

    uint8_t op = 0;       // opcode
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
    uint8_t cycles_last_taken = 0;
    bool  cpuHalted = false;
    uint8_t prevInterruptState = 0;

    // Interrupts
    bool ime = true;
    bool ime_prev = true;
    bool halt_bug_executed = false;
    uint8_t EI_counter = 0;

  private:
    Bus* bus;
    Ppu* ppu;

  public:
    // Other flags
    bool divwrite_timer = false;
    Debugger* debugger;
    bool gbdoc = false;
    bool debug = false;

  public:
    // Array of function pointers to 8-bit opcodes.
    typedef void (Cpu::*Opcode_8bit)();
    Opcode_8bit opcodes_8bit[256] = { 
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

    const char* opcode_8bit_names[256] = { 
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
    uint8_t handleInterrupt();
    void tick();

    // =========== 8-bit ===============
    // Rotate/shift
    void RLA();
    void RRA();
    void RLCA();
    void RRCA();
    
    // Bit ops
    void AND_A_A();
    void AND_A_B();
    void AND_A_C();
    void AND_A_D();
    void AND_A_E();
    void AND_A_H();
    void AND_A_L();
    void AND_A_atHL();
    void AND_A_u8();

    void OR_A_A();
    void OR_A_B();
    void OR_A_C();
    void OR_A_D();
    void OR_A_E();
    void OR_A_H();
    void OR_A_L();
    void OR_A_atHL();
    void OR_A_u8();

    void XOR_A_A();
    void XOR_A_B();
    void XOR_A_C();
    void XOR_A_D();
    void XOR_A_E();
    void XOR_A_H();
    void XOR_A_L();
    void XOR_A_atHL();
    void XOR_A_u8();

    // 8-bit ALU
    void INC_A();
    void INC_B();
    void INC_C();
    void INC_D();
    void INC_E();
    void INC_H();
    void INC_L();
    void INC_atHL();

    void DEC_A();
    void DEC_B();
    void DEC_C();
    void DEC_D();
    void DEC_E();
    void DEC_H();
    void DEC_L();
    void DEC_atHL();

    void ADD_A_A();
    void ADD_A_B();
    void ADD_A_C();
    void ADD_A_D();
    void ADD_A_E();
    void ADD_A_H();
    void ADD_A_L();
    void ADD_A_atHL();
    void ADD_A_u8();

    void SUB_A_A();
    void SUB_A_B();
    void SUB_A_C();
    void SUB_A_D();
    void SUB_A_E();
    void SUB_A_H();
    void SUB_A_L();
    void SUB_A_atHL();
    void SUB_A_u8();

    void SBC_A_A();
    void SBC_A_B();
    void SBC_A_C();
    void SBC_A_D();
    void SBC_A_E();
    void SBC_A_H();
    void SBC_A_L();
    void SBC_A_atHL();
    void SBC_A_u8();

    void ADC_A_A();
    void ADC_A_B();
    void ADC_A_C();
    void ADC_A_D();
    void ADC_A_E();
    void ADC_A_H();
    void ADC_A_L();
    void ADC_A_atHL();
    void ADC_A_u8();

    void CP_A_A();
    void CP_A_B();
    void CP_A_C();
    void CP_A_D();
    void CP_A_E();
    void CP_A_H();
    void CP_A_L();
    void CP_A_atHL();
    void CP_A_u8();

    // 16-bit ALU
    void DEC_BC();
    void DEC_DE();
    void DEC_HL();
    void DEC_SP();
    void INC_BC();
    void INC_DE();
    void INC_HL();
    void INC_SP();
    void ADD_HL_BC();
    void ADD_HL_DE();
    void ADD_HL_HL();
    void ADD_HL_SP();
    void ADD_SP_i8();

    // 8-bit loads
    void LD_A_atHL();
    void LD_B_atHL();
    void LD_C_atHL();
    void LD_D_atHL();
    void LD_E_atHL();
    void LD_H_atHL();
    void LD_L_atHL();

    void LD_atHL_A();
    void LD_atHL_B();
    void LD_atHL_C();
    void LD_atHL_D();
    void LD_atHL_E();
    void LD_atHL_H();
    void LD_atHL_L();
    void LD_atHL_u8();

    void LD_A_u8();
    void LD_B_u8();
    void LD_C_u8();
    void LD_D_u8();
    void LD_E_u8();
    void LD_H_u8();
    void LD_L_u8();

    void LD_A_A();   // Register to register
    void LD_A_B();
    void LD_A_C();
    void LD_A_D();
    void LD_A_E();
    void LD_A_H();
    void LD_A_L();
    void LD_B_A();
    void LD_B_B();
    void LD_B_C();
    void LD_B_D();
    void LD_B_E();
    void LD_B_H();
    void LD_B_L();
    void LD_C_A();
    void LD_C_B();
    void LD_C_C();
    void LD_C_D();
    void LD_C_E();
    void LD_C_H();
    void LD_C_L();
    void LD_D_A();
    void LD_D_B();
    void LD_D_C();
    void LD_D_D();
    void LD_D_E();
    void LD_D_H();
    void LD_D_L();
    void LD_E_A();
    void LD_E_B();
    void LD_E_C();
    void LD_E_D();
    void LD_E_E();
    void LD_E_H();
    void LD_E_L();
    void LD_H_A();
    void LD_H_B();
    void LD_H_C();
    void LD_H_D();
    void LD_H_E();
    void LD_H_H();
    void LD_H_L();
    void LD_L_A();
    void LD_L_B();
    void LD_L_C();
    void LD_L_D();
    void LD_L_E();
    void LD_L_H();
    void LD_L_L();

    void LD_BC_A();
    void LD_atBC_A();
    void LD_atDE_A();
    void LD_A_atBC();
    void LD_A_atDE();

    void LD_atu16_A();
    void LD_A_atu16();

    void LDH_atu8_A();
    void LDH_atC_A();
    void LDH_A_atu8();
    void LDH_A_atC();

    void LDD_A_atHL();
    void LDD_atHL_A();
    void LDI_A_atHL();
    void LDI_atHL_A();

    // 16-bit loads
    void LD_BC_u16();
    void LD_DE_u16();
    void LD_HL_u16();
    void LD_SP_u16();
    void LD_atu16_SP();
    void LD_SP_HL();
    void LD_HL_SP_i8();
    void PUSH_AF();
    void PUSH_BC();
    void PUSH_DE();
    void PUSH_HL();
    void POP_AF();
    void POP_BC();
    void POP_DE();
    void POP_HL();

    // Jumps
    void JP_HL();
    void JR_i8();
    void JP_u16();
    void JP_NZ_u16();
    void JP_Z_u16();
    void JP_NC_u16();
    void JP_C_u16();
    void JR_NZ_i8();
    void JR_Z_i8();
    void JR_NC_i8();
    void JR_C_i8();

    // Calls
    void CALL_NZ_u16();
    void CALL_Z_u16();
    void CALL_u16();
    void CALL_NC_u16();
    void CALL_C_u16();

    // Returns
    void RET_NZ();
    void RET_Z();
    void RET();
    void RETI();
    void RET_NC();
    void RET_C();

    // Restart
    void RST_00h();
    void RST_08h();
    void RST_10h();
    void RST_18h();
    void RST_20h();
    void RST_28h();
    void RST_30h();
    void RST_38h();

    // Misc
    void CPL();
    void CCF();
    void SCF();
    void NOP();
    void HALT(); // !!!
    void STOP(); // !!!
    void DI();
    void EI();
    void DAA();

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
    void SetAddFlags(uint16_t a, uint16_t b, bool setzero);
    void SetSubFlags(uint16_t a, uint16_t b);

    void LD_n_atHL(uint8_t * reg);
    void LD_atHL_n(uint8_t * reg);
    void LD_n_u8(uint8_t * reg);
    void LD_n_m(uint8_t * dst, uint8_t * src);
    void LD_n_atM(uint8_t * src, uint16_t address);
    void Push_NM(uint16_t regpair);
    void Pop_NM(uint8_t * upper, uint8_t * lower);

    void INC_n(uint8_t * reg);
    void DEC_n(uint8_t * reg);
    void ADD_A_n(uint8_t * reg);
    void SUB_A_n(uint8_t * reg);
    void SBC_A_n(uint8_t * reg);
    void ADC_A_n(uint8_t * reg);
    void CP_A_n(uint8_t *reg);
    void DEC_nm(uint8_t * upper_reg, uint8_t * lower_reg);
    void INC_nm(uint8_t * upper_reg, uint8_t * lower_reg);
    void ADD_HL_nm(uint8_t * upper, uint8_t * lower);
    void JP_flag_u16(CpuFlags flag, uint8_t val);
    void CALL_flag_u16(CpuFlags flag, uint8_t val);
    void JR_flag_i8(CpuFlags flag, uint8_t val);
    void AND_A_n(uint8_t * reg);
    void OR_A_n(uint8_t * reg);
    void XOR_A_n(uint8_t * reg);

    uint8_t   Pop8Bit(void);
    uint16_t  Pop16Bit(void);
    void Push8Bit(uint8_t val);
    void Push16Bit(uint16_t val);

    uint8_t   FetchNextByte(void);

    // =========== 16-bit ===============
    void Decode16bitOpcode(uint8_t op);
    void RLC(uint8_t * reg);
    void RRC(uint8_t * reg);
    void RL(uint8_t * reg);
    void RR(uint8_t * reg);
    void SLA(uint8_t * reg);
    void SRA(uint8_t * reg);
    void SWAP(uint8_t * reg);
    void SRL(uint8_t * reg);
    void BIT_n(uint8_t * reg, uint8_t bitpos);
    void RES_n(uint8_t * reg, uint8_t bitpos);
    void SET_n(uint8_t * reg, uint8_t bitpos);

    void RLC_atHL();
    void RRC_atHL();
    void RL_atHL();
    void RR_atHL();
    void SLA_atHL();
    void SRA_atHL();
    void SWAP_atHL();
    void SRL_atHL();
    void BIT_n_atHL(uint8_t bitpos);
    void RES_n_atHL(uint8_t bitpos);
    void SET_n_atHL(uint8_t bitpos);

  public:
    uint8_t execute();
    void    runTimer();

    bool GetFlag(CpuFlags flag);
    void AssignFlag(CpuFlags flag, bool val);
    void unknown(uint8_t opcode);
    void AddCycles(uint8_t cycles) { cycleCount += cycles; }
    uint8_t GetCycles() { return cycleCount; }

    // Set register equal to a value
    void af(uint16_t af);
    void bc(uint16_t bc);
    void de(uint16_t de);
    void hl(uint16_t hl);
   
    // Return value of registers
    uint16_t af() const { return (a << 8) | f; }
    uint16_t bc() const { return (b << 8) | c; }
    uint16_t de() const { return (d << 8) | e; }
    uint16_t hl() const { return (h << 8) | l; }

    uint16_t getSp() const { return sp; }
    uint16_t getPc() const { return pc; }
    uint8_t getOp() const { return op; }
    uint8_t get_last_cycles() const { return cycles_last_taken; }

  public:
    Cpu(Bus* bus_, Debugger* debug_, Ppu* ppu_) {
      bus = bus_;
      debugger = debug_;
      ppu = ppu_;
    }
};

#endif
