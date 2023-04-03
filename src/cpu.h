
/* █▀▀ █▀█ █░█ */
/* █▄▄ █▀▀ █▄█ */

#ifndef CPU_H
#define CPU_H

#include <stdio.h>
#include <array>
#include <fstream>
#include <vector>

#include "common.h"

// Using t-cycles
#define MEM_RW_CYCLES 4
#define ALU_CYCLES 4
#define NOP_CYCLES 4

// Timer control
#define TAC_ENABLE_BIT 0x2
#define TAC_ENABLE_MASK(x) (((unsigned) x >> TAC_ENABLE_BIT) & 0x1)
#define TAC_SELECT_MASK(x) (((unsigned) x & 0b11))
#define TAC_1024 0b00
#define TAC_16   0b01
#define TAC_64   0b10
#define TAC_256  0b11

#define TMA_RELOAD 4 // 1 m cycle

// Interrupts
#define INTR_TYPES 5

#define INTR_VBLNK 0
#define INTR_STAT 1
#define INTR_TMR 2
#define INTR_SRL 3
#define INTR_JOYP 4
const u8 Intr_Bits[5] = {
  INTR_VBLNK,
  INTR_STAT,
  INTR_TMR,
  INTR_SRL,
  INTR_JOYP
};

#define ISR_ADDR_VBLANK 0x40
#define ISR_ADDR_STAT 0x48
#define ISR_ADDR_TMR 0x50
#define ISR_ADDR_SRL 0x58
#define ISR_ADDR_JOYP 0x60

const u8 Intr_Addr[5] = {
  ISR_ADDR_VBLANK,
  ISR_ADDR_STAT,
  ISR_ADDR_TMR,
  ISR_ADDR_SRL,
  ISR_ADDR_JOYP
};

/* Joypad */
#define JOYP_SEL_MASK   0b00110000
#define JOYP_SEL_ACTION 5
#define JOYP_SEL_DIR    4
#define JOYP_D_STRT 3
#define JOYP_U_SEL  2
#define JOYP_L_B    1
#define JOYP_R_A    0

// Disassembly table stuff
typedef enum DT_ALU_Type {
  ALU_ADD_A,  ALU_ADC_A,  ALU_SUB,  ALU_SBC_A,
  ALU_AND,    ALU_XOR,    ALU_OR,   ALU_CP,
} DT_ALU_Type;

const DT_ALU_Type DT_alu[8] = {
  ALU_ADD_A,  ALU_ADC_A,  ALU_SUB,  ALU_SBC_A,
  ALU_AND,    ALU_XOR,    ALU_OR,   ALU_CP,
};

typedef enum DT_Rot_Type {
  DT_RLC, DT_RRC, DT_RL,   DT_RR,
  DT_SLA, DT_SRA, DT_SWAP, DT_SRL
} DT_Rot_Type;

const DT_Rot_Type DT_rot[8] = {
  DT_RLC, DT_RRC, DT_RL,   DT_RR,
  DT_SLA, DT_SRA, DT_SWAP, DT_SRL
};

typedef enum Cpu_States {
  CPU_NORMAL,
  CPU_HALT_IME_SET,
  CPU_HALT_IME_NOT_SET,
  CPU_EI,
  CPU_HALT_BUG,
  CPU_STOP,
} Cpu_States;

static const char* Cpu_StatesStr[7] = {
  "CPU_NORMAL",
  "CPU_HALT_IME_SET",
  "CPU_HALT_IME_NOT_SET",
  "CPU_EI",
  "CPU_HALT_BUG",
  "CPU_STOP",
};

typedef u16 Address;
typedef u8 Register;
typedef bool Flag;

class Bus;
class Ppu;
class Debugger;

class Cpu {
  private:
    u8 a = 0x01;
    u8 b = 0xFF;
    u8 c = 0x13;
    u8 d = 0x00;
    u8 e = 0xC1;
    u8 h = 0x84;
    u8 l = 0x03;

    struct f {
      bool Z = false;  // Zero
      bool N = false; // Subtract
      bool HC = false;  // Half carry
      bool C = false;  // Carry
    } f;

    u8 op = 0x00;
    u16 sysclk = 0x00;
    u16 sp = 0xFFFE;
    u16 pc = 0x0100;

    bool ime = true;
    bool prevIme = true;
    u8 prevIntrState = 0;

    Cpu_States cpuState = CPU_NORMAL;

  // Pointers to commonly used stuff
  // figured it might be faster than a whole call to bus->read
  private:
    u8 * divPtr;
    u8 * joypPtr;
    u8 * intf;
    u8 * inte;

  private:
    u16 const inline af() { return (a << 8) | GetFlagsAsInt(); }
    u16 const inline bc() { return b << 8 | c; }
    u16 const inline de() { return d << 8 | e; }
    u16 const inline hl() { return h << 8 | l; }

    void inline af(u8 x, u8 y) { a = x; SetFlags(y); }
    void inline bc(u8 x, u8 y) { b = x; c = y; }
    void inline de(u8 x, u8 y) { d = x; e = y; }
    void inline hl(u8 x, u8 y) { h = x; l = y; }

    void inline af(u16 x) { a = x >> 8; SetFlags(x & 0xFF); }
    void inline bc(u16 x) { b = x >> 8; c = x & 0xFF; }
    void inline de(u16 x) { d = x >> 8; e = x & 0xFF; }
    void inline hl(u16 x) { h = x >> 8; l = x & 0xFF; }

    void SetFlags(bool z, bool n, bool hc, bool c);
    void SetFlags(u8 flags);
    u8 const GetFlagsAsInt();

    void Decode8BitOpcode();
    void Decode16BitOpcode();

    void Tick(u8 cycles);

    u8 MemReadRaw(Address addr);
    u8 MemRead_u8(Address * addr);
    u16 MemRead_u16(Address * addr);
    void MemWriteRaw(Address addr, u8 value);
    void MemWrite_u8(Address * addr, u8 value);
    void MemWrite_u16(Address * addr, u16 value);
    void Push_u8(u8 value);
    void Push_u16(u16 value);

    void SetFlags_16bitAdd_C(u16 x, u16 y);
    void SetFlags_16bitAdd_HC(u16 x, u16 y);
    void SetFlags_8bitAdd_C(u8 x, u8 y);
    void SetFlags_8bitAdd_HC(u8 x, u8 y);

    void SetFlags_16bitSub_C(u16 x, u16 y);
    void SetFlags_16bitSub_HC(u16 x, u16 y);
    void SetFlags_8bitSub_C(u8 x, u8 y);
    void SetFlags_8bitSub_HC(u8 x, u8 y);

    void HandleInterrupt();
    void RunTimer(u8 cycles);

    bool doTMAreload = false;
    bool doneTMAreload = false;
    u8 tmaReload = 0;

    u8 joypSelection = JOYP_SEL_NIL_VAL;
    u8 keyvec_dir = 0x0F;
    u8 keyvec_act = 0x0F;

  public: // make private later
    Bus * bus;
    Ppu * ppu;
    Debugger * debugger;

  public:
    void Init();
    void Execute();
    void RunInstruction();
    void Key_Up(KeyType type, Keys key);
    void Key_Down(KeyType type, Keys key);

  // Flags
  public:
    bool gbdoc = false; // regdump
    bool step = false; // step 1 instruction
    bool doLog = false;

  public:
    Cpu(Bus* bus_, Ppu* ppu_, Debugger * debugger_) {
      bus = bus_;
      ppu = ppu_;
      debugger = debugger_;
    }

  /* Instructions */
  private:
    /* Disassembly tables */
    Flag * DT_cc[4] = {
      &(f.Z), &(f.Z),
      &(f.C), &(f.C),
    };

    // NULL is (HL) - handle manually within opcode
    Register * DT_r[8] = {
      &b, &c, &d,   &e,
      &h, &l, NULL, &a,
    };

    bool doDMATransfer = false;
    Address dmaAddr;
    u8 dmaByteCnt;
    void DMA_Transfer();

    void NOP();
    void LD_atNN_SP();
    void STOP();
    void JR_s8();
    void JR_cc_s8(Flag * f, bool cond);
    void LD_rp_NN(u8 p);
    void ADD_HL_rp(u8 p);
    void LD_atBC_A();
    void LD_atDE_A();
    void LD_atHLi_A();
    void LD_atHLd_A();
    void LD_A_atBC();
    void LD_A_atDE();
    void LD_A_atHLi();
    void LD_A_atHLd();
    void INC_rp(u8 p);
    void DEC_rp(u8 p);
    void INC_r(Register * x);
    void DEC_r(Register * x);
    void LD_r_d8(Register * x);
	  void RLCA();
    void DAA();
	  void RRCA();
    void CPL();
	  void RLA();
    void SCF();
	  void RRA();
    void CCF();
    void LD_r1_r2(Register * x, Register * y);
    void HALT();
    void ALU_r(DT_ALU_Type aluType, Register * r);
    void RET_cc(Flag * flag, bool cond);
    void LD_a8_A();
    void ADD_SP_s8();
    void LD_A_a8();
    void LD_HL_SPs8();
    void POP_rp2(u8 p);
    void RET();
    void RETI();
    void JP_HL();
    void LD_SP_HL();
    void JP_cc_d16(Flag * cc, bool cond);
    void LD_A_atC();   
    void LD_a16_A(); 
    void LD_atC_A();   
    void LD_A_a16(); 
    void JP_nn();
    void EI();
    void DI();
    void CALL_cc_a16(Flag * cc, bool cond);
    void PUSH_rp2(u8 p);
    void CALL_a16();
    void ALU_n(DT_ALU_Type aluType);
    void RST_y(Address addr);
    void ROT_y_z(DT_Rot_Type rotType, Register * x);
    void BIT_y_r(u8 n, Register * x);
    void RES_y_r(u8 n, Register * x);
    void SET_y_r(u8 n, Register * x);

    // Mainly for debugging
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

  friend class Bus;
  friend class Debugger;
};

#endif
