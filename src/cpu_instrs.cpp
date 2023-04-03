
#include "common.h"
#include "cpu.h"
#include "bus.h"

/* 00: NOP */
// void Cpu::NOP() { }

/* 08: LD (a16), SP */
void Cpu::LD_atNN_SP() {
  Address addr = MemRead_u16(&pc);
  MemWrite_u16(&addr, sp);
}

/* 10: STOP
 * Halt CPU and LCD until button pressed.
 * Follows Pandocs STOP flowchart */
void Cpu::STOP() {
  u8 ie = MemReadRaw(INTE);
  u8 irq = MemReadRaw(INTF);
  u8 intrPending = ie & irq;

  bus->Write(DIV, 0);
  cpuState = CPU_STOP;

  if (!intrPending) {
    ++pc;
  }
}

/* 18: JR s8 */
void Cpu::JR_s8() {
  int8_t s8 = MemRead_u8(&pc);
  Tick(ALU_CYCLES);
  pc += s8;
}

/* 20 28 30 38: JR cc s8 */
void Cpu::JR_cc_s8(Flag * f, bool cond) {
  int8_t s8 = MemRead_u8(&pc);
  if (*f == cond) {
    Tick(ALU_CYCLES);
    pc += s8;
  }
}

/* 01 11 21 31: LD rp, d16 
 * Load 2 bytes of immediate data into register pair. */
void Cpu::LD_rp_NN(u8 p) {
  u8 lsb = MemRead_u8(&pc);
  u8 msb = MemRead_u8(&pc);

  switch (p) {
    case 0: b = msb; c = lsb; break;
    case 1: d = msb; e = lsb; break;
    case 2: h = msb; l = lsb; break;
    case 3: sp = (msb << 8) | lsb; break;
    default: break;
  }
}

/* 09 19 29 39: ADD HL, rp */
void Cpu::ADD_HL_rp(u8 p) {
  u16 rp;
  switch (p) {
    case 0: rp = bc(); break;
    case 1: rp = de(); break;
    case 2: rp = hl(); break;
    case 3: rp = sp; break;
    default: break;
  }

  Tick(ALU_CYCLES);
  f.N = false;
  SetFlags_16bitAdd_C(rp, hl());
  SetFlags_16bitAdd_HC(rp, hl());

  u16 sum = hl() + rp;
  h = sum >> 8;
  l = sum & 0xFF;
}

/* 02: LD (BC), A */
void Cpu::LD_atBC_A() {
  Address addr = bc();
  MemWrite_u8(&addr, a);
}

/* 12: LD (DE), A*/
void Cpu::LD_atDE_A() {
  Address addr = de();
  MemWrite_u8(&addr, a);
}

/* 22: LD (HL+), A */
void Cpu::LD_atHLi_A() {
  Address addr = hl();
  MemWrite_u8(&addr, a);
  hl(addr);
}

/* 3A: LD (HL-), A */
void Cpu::LD_atHLd_A() {
  Address addr = hl();
  MemWrite_u8(&addr, a);
  hl(addr-2);
}

/* 0A: LD A, (BC)
 * Load the 8-bit contents of memory specified by
 * register pair BC into register A.
 * */
void Cpu::LD_A_atBC() {
  Address addr = bc();
  a = MemRead_u8(&addr);
}

/* 1A: LD A, (DE) */
void Cpu::LD_A_atDE() {
  Address addr = de();
  a = MemRead_u8(&addr);
}

/* 2A: LD A, (HL+) */
void Cpu::LD_A_atHLi() {
  Address addr = hl();
  a = MemRead_u8(&addr);
  hl(addr);
}

/* 3A: LD A, (HL-) */
void Cpu::LD_A_atHLd() {
  Address addr = hl();
  a = MemRead_u8(&addr);
  hl(addr - 2);
}

/* 03 13 23 33: INC rp */
void Cpu::INC_rp(u8 p) {
  Tick(ALU_CYCLES);
  u16 rp;
  switch(p) {
    case 0: 
      rp = bc();
      bc(++rp);
      break;
    case 1:
      rp = de();
      de(++rp);
      break;
    case 2:
      rp = hl();
      hl(++rp);
      break;
    case 3:
      ++sp;
      break;
    default: break;
  }
}

/* 0B 1B 2B 3B: INC rp */
void Cpu::DEC_rp(u8 p) {
  Tick(ALU_CYCLES);
  u16 rp;
  switch(p) {
    case 0: 
      rp = bc();
      bc(--rp);
      break;
    case 1:
      rp = de();
      de(--rp);
      break;
    case 2:
      rp = hl();
      hl(--rp);
      break;
    case 3:
      --sp;
      break;
    default: break;
  }
}

/* 04 14 24 34 0C 1C 2C 3C: INC r */
void Cpu::INC_r(Register * x) {
  // (HL) handled manually
  if (x == NULL) {
    Address addr = hl();
    u8 val = MemRead_u8(&addr);
    SetFlags_8bitAdd_HC(val, 1);
    val++;
    f.N = false;
    f.Z = val == 0;
    addr = hl();
    MemWrite_u8(&addr, val);
    return;
  }

  SetFlags_8bitAdd_HC(*x, 1);
  (*x)++;
  f.N = false;
  f.Z = *x == 0;
}

/* 05 15 25 35 0D 1D 2D 3D: DEC r */
void Cpu::DEC_r(Register * x) {
  // (HL) handled manually
  if (x == NULL) {
    Address addr = hl();
    u8 val = MemRead_u8(&addr);
    addr = hl();
    MemWrite_u8(&addr, val - 1);

    f.N = true;
    f.Z = (val-1) == 0;
    SetFlags_8bitSub_HC(val, 1);
    return;
  }

  SetFlags_8bitSub_HC(*x, 1);
  (*x)--;
  f.N = true;
  f.Z = *x == 0;
}

/* 06 0E 16 1E 26 2E 36 3E: LD r, d8
 * Load 8-bit immediate operand d8 into register R. */
void Cpu::LD_r_d8(Register * x) {
  u8 d8 = MemRead_u8(&pc);

  // (HL) handled separately
  // Store d8 into address (HL)
  if (x == NULL) {
    Address addr = hl();
    MemWrite_u8(&addr, d8);
    return;
  }

  *x = d8;
}

/* 07: RLCA
 * Rotate the contents of register A to the left. 
 * The contents of bit 7 are placed in both the CY
 * flag and bit 0 of register A. */
void Cpu::RLCA() {
  u8 oldbit7 = a >> 7;
  a <<= 1;
  a |= oldbit7;

  f.Z = 0;
  f.N = 0;
  f.HC = 0;
  f.C = oldbit7;
}

/* 27: DAA
 * Called immediately after 2 BCD-encoded digits are added.
 * Adjusts accumulator to correct BCD representation.
 * Sets carry flag if result > 0x99. */
void Cpu::DAA() {
  u16 correction = 0; 
  
  u8 HC = f.HC;
  u8 C = f.C;
  u8 N = f.N;
  
  if (HC || (!N && ((a & 0xF) > 0x9))) {
    correction |= 0x6;
  }
  
  if (C || (!N && (a > 0x99))) {
    correction |= 0x60;
    f.C = true;
  }

  a += N ? -correction : correction;
  
  f.HC = 0;
  f.Z = a == 0;
}

/* 0F: RRCA
 * Rotate A right. Old bit 0 to CY and bit 7 of A. */
void Cpu::RRCA() {
  u8 oldbit0 = a & 0x1;
  a >>= 1;
  a |= (oldbit0 << 7);

  f.Z = 0;
  f.N = 0;
  f.HC = 0;
  f.C = oldbit0;
}

/* 2F: CPL
 * Complement register A. */
void Cpu::CPL() {
  a = ~a;
  f.N = 1;
  f.HC = 1;
}

/* 17: RLA
 * Combine CY and A to make a 9-bit number with CY as msb.
 * Then rotate that left. Old CY gets copied to bit 0.
 * Old bit 8 of A is discarded. */
void Cpu::RLA() {
  u8 oldCy = f.C;
  u8 oldbit7 = a >> 7;
  a <<= 1;
  a |= oldCy;

  f.Z = 0;
  f.N = 0;
  f.HC = 0;
  f.C = oldbit7;
}

/* 37: SCF
 * Set carry flag. */
void Cpu::SCF() {
  f.HC = 0;
  f.N = 0;
  f.C = 1;
}

/* 1F: RRA
 * Combine CY and A to make a 9-bit number, with CY as msb.
 * Then rotate that right. Old CY gets copied to bit 7.
 * Old bit 0 of A is discarded. */
void Cpu::RRA() {
  u8 oldCy = f.C;
  u8 oldbit0 = a & 0x1;
  a >>= 1;
  a |= (oldCy << 7);

  f.Z = 0;
  f.N = 0;
  f.HC = 0;
  f.C = oldbit0;
}

/* 3F: CCF
* Complement carry flag. */
void Cpu::CCF() {
  f.N = 0;
  f.HC = 0;
  f.C = !f.C;
}

/* 40-7F: LD R1, R2
 * Set R1 = R2 */
void Cpu::LD_r1_r2(Register * x, Register * y) {
  // Handle (HL) manually
  // Store contents of other register into (HL)
  if (x != NULL && y == NULL) {
    Address addr = hl();
    *x = MemRead_u8(&addr);
    return;
  } else if (x == NULL & y != NULL) {
    Address addr = hl();
    MemWrite_u8(&addr, *y);
    return;
  }

  *x = *y;
}

/* 76: HALT */
void Cpu::HALT() {
  prevIntrState = MemReadRaw(INTE) & MemReadRaw(INTF);

  // When EI is followed immediately by HALT and interrupt is
  // pending as HALT is executed, the interrupt is serviced
  // but the interrupt returns to HALT
  if (prevIntrState && cpuState == CPU_EI) {
    --pc;
    return;
  }
  
  if (ime) {
    cpuState = CPU_HALT_IME_SET;
  } else {
    cpuState = CPU_HALT_IME_NOT_SET;
  }
}

/* Operate on acc and register/memory location */
void Cpu::ALU_r(DT_ALU_Type aluType, Register * r) {
  u8 reg;
  if (r == NULL) {
    Address addr = hl();
    reg = MemRead_u8(&addr);
  } else {
    reg = *r;
  }

  if (aluType == ALU_ADD_A) {
    SetFlags_8bitAdd_C(a, reg);
    SetFlags_8bitAdd_HC(a, reg);
    a += reg;
    f.Z = a == 0;
    f.N = false;

  // A = Reg R + CY
  } else if (aluType == ALU_ADC_A) {
    // Can't use SetFlags functions because they're
    // for adding only 2 numbers
    f.N = 0;

    u16 hc_sum = (a & 0xF) + (reg & 0xF) + (f.C & 0xF);
    f.HC = hc_sum > 0xF;

    u16 sum = a + reg + f.C;
    f.C = sum > 0xFF;

    a = sum & 0xFF;
    f.Z = a == 0;

  } else if (aluType == ALU_SUB) {
    SetFlags_8bitSub_C(a, reg);
    SetFlags_8bitSub_HC(a, reg);
    a -= reg;
    f.Z = a == 0;
    f.N = true;

  // A = Reg R - CY
  } else if (aluType == ALU_SBC_A) {
    // Can't use SetFlags functions because they're
    // for subtracting only 2 numbers
    f.N = true;

    u16 hc_diff = (a & 0xF) - ((reg & 0xF) + f.C);
    f.HC = hc_diff & 0x10;

    u16 diff = a - (reg + f.C);
    f.C = diff > 0xFF;

    a = diff & 0xFF;
    f.Z = a == 0;
 
  } else if (aluType == ALU_AND) {
    a &= reg;
    f.Z = a == 0;
    f.N = 0;
    f.HC = 1;
    f.C = 0;

  } else if (aluType == ALU_XOR) {
    a ^= reg;
    f.Z = a == 0;
    f.N = 0;
    f.HC = 0;
    f.C = 0;

  } else if (aluType == ALU_OR) {
    a |= reg;
    f.Z = a == 0;
    f.N = 0;
    f.HC = 0;
    f.C = 0;

  // Compare Reg A and Reg r by checking A - r == 0
  } else if (aluType == ALU_CP) {
    SetFlags_8bitSub_C(a, reg);
    SetFlags_8bitSub_HC(a, reg);
    f.Z = (a - reg) == 0;
    f.N = true;
  }
}

/* C0 C8 D0 D8: RET cc
 * If flag is set, return from function. */
void Cpu::RET_cc(Flag * flag, bool cond) {
  if (*flag == cond) {
    u8 lsb = MemRead_u8(&sp);
    Tick(ALU_CYCLES);
    u8 msb = MemRead_u8(&sp);
    Tick(ALU_CYCLES);
    pc = (msb << 8) | lsb;
  } else {
    // NOTE: I don't actually know what takes up this cycle
    Tick(ALU_CYCLES);
  }
}

/* E0: LD (FF00 + a8), A
 * Store contents of Reg A into (FF00 + a8) */
void Cpu::LD_a8_A() {
  u8 a8 = MemRead_u8(&pc);
  Address addr = 0xFF00 + a8;
  MemWrite_u8(&addr, a);
}

/* E8: ADD SP, s8
 * Add 8-bit signed immediate to sp and store result in sp.
 * Half-carry flag is set from bit 3 to 4 and carry is set
 * from bit 7 because it's treated as 2 8-bit ops. */
void Cpu::ADD_SP_s8() {
  int8_t i8 = MemRead_u8(&pc);

  u16 hc_sum = (sp & 0xF) + (i8 & 0xF);
  f.HC = (hc_sum & 0x10) == 0x10;

  u16 c_sum = (sp & 0xFF) + (i8 & 0xFF);
  f.C = (c_sum & 0x100) == 0x100;
  sp += i8;

  f.Z = 0;
  f.N = 0;
}

/* F0: LD A, (FF00 + a8) */
void Cpu::LD_A_a8() {
  Address addr = 0xFF00 + MemRead_u8(&pc);
  a = MemRead_u8(&addr);
}

/* F8: LD HL, SP+s8 */
void Cpu::LD_HL_SPs8() {
  int8_t i8 = MemRead_u8(&pc);

  u16 hc_sum = (sp & 0xF) + (i8 & 0xF);
  f.HC = (hc_sum & 0x10) == 0x10;

  u16 c_sum = (sp & 0xFF) + (i8 & 0xFF);
  f.C = (c_sum & 0x100) == 0x100;

  f.Z = 0;
  f.N = 0;

  hl(sp + i8);
}

/* C1 D1 E1 F1: POP rp2 */
void Cpu::POP_rp2(u8 p) {
  u16 val = MemRead_u16(&sp);

  switch (p) {
    case 0: bc(val); break;
    case 1: de(val); break;
    case 2: hl(val); break;
    case 3: af(val); break;
    default: break;
  }
}

/* C9: RET
 * Return from function call */
void Cpu::RET() {
  pc = MemRead_u16(&sp);
  Tick(ALU_CYCLES); // idk what actually makes it tick
}

/* D9: RETI
 * Used when an ISR finishes. */
void Cpu::RETI() {
  ime = prevIme;
  pc = MemRead_u16(&sp);
  bus->Write(INTF, 0); // TODO VERY BAD FIX LATER
  Tick(ALU_CYCLES);
}

/* E9: JP HL */
void Cpu::JP_HL() {
  pc = hl();
}

/* F9: LD SP, HL */
void Cpu::LD_SP_HL() {
  sp = hl();
  Tick(ALU_CYCLES);
}

/* C2 CA D2 DA: JP cc d16 */
void Cpu::JP_cc_d16(Flag * cc, bool cond) {
  u16 newPc = MemRead_u16(&pc);
  if (*cc == cond) {
    pc = newPc;
    Tick(ALU_CYCLES);
  }
}

/* F2: LD A, (C) */
void Cpu::LD_A_atC() {
  Address addr = 0xFF00 | c;
  a = MemRead_u8(&addr);
}

/* E2: LD (C), A */
void Cpu::LD_atC_A() {
  Address addr = 0xFF00 | c;
  MemWrite_u8(&addr, a);
}

/* EA: LD (a16), A */
void Cpu::LD_a16_A() {
  Address addr = MemRead_u16(&pc);
  MemWrite_u8(&addr, a);
}

/* FA: LD A, (a16) */
void Cpu::LD_A_a16() {
  Address addr = MemRead_u16(&pc);
  a = MemRead_u8(&addr);
}

/* C4: JP a16 */
void Cpu::JP_nn() {
  pc = MemRead_u16(&pc);
  Tick(ALU_CYCLES);
}

/* FB: EI */
void Cpu::EI() {
  if (cpuState != CPU_EI) {
    cpuState = CPU_EI;
  }
}

/* F3: DI */
void Cpu::DI() {
  ime = false;
}

/* C4 CC D4 DC CALL cc, a16 */
void Cpu::CALL_cc_a16(Flag * cc, bool cond) {
  u16 newPc = MemRead_u16(&pc);
  if (*cc == cond) {
    Push_u16(pc);
    pc = newPc;
    Tick(ALU_CYCLES);
  }
}

/* C5 D5 E5 F5 PUSH rp2 */
void Cpu::PUSH_rp2(u8 p) {
  switch (p) {
    case 0: Push_u16(bc()); break;
    case 1: Push_u16(de()); break;
    case 2: Push_u16(hl()); break;
    case 3: Push_u16(af()); break; 
    default: break;
  }
}

/* CD: CALL a16 */
void Cpu::CALL_a16() {
  Address addr = MemRead_u16(&pc);
  Push_u16(pc);
  Tick(ALU_CYCLES);
  pc = addr;
}

/* C6 CE D6 DE E6 EE F6 FE ALU n */
void Cpu::ALU_n(DT_ALU_Type aluType) {
  u8 d8 = MemRead_u8(&pc);
  ALU_r(aluType, &d8);
}

/* C8 CF D8 DF E8 EF F8 FF RST_y */
void Cpu::RST_y(Address addr) {
  Push_u16(pc);
  Tick(MEM_RW_CYCLES);
  pc = addr;
}


/* ▄█ █▄▄ ▄▄ █▄▄ █ ▀█▀    █▀█ █▀█ █▀ */
/* ░█ █▄█ ░░ █▄█ █ ░█░    █▄█ █▀▀ ▄█ */

void Cpu::ROT_y_z(DT_Rot_Type rot, Register * x) {
  // If x is null, that means we should operate on (HL)
  if (x == NULL) {
    x = bus->GetAddressPointer(hl());
  }

  // Rotate left
  if (rot == DT_RLC) {
    u8 oldbit7 = *x >> 7;
    *x <<= 1;
    *x |= oldbit7;
    f.C = oldbit7;
    f.Z = *x == 0;
    f.HC = false;
    f.N = false;
  }

  // Rotate right
  else if (rot == DT_RRC) {
    u8 oldbit0 = *x & 0x1;
    *x >>= 1;
    *x |= oldbit0 << 7;
    f.C = oldbit0;
    f.Z = *x == 0;
    f.HC = false;
    f.N = false;
  }

  // Rotate left
  else if (rot == DT_RL) {
    u8 oldbit7 = *x >> 7;
    *x <<= 1;
    *x |= f.C;

    f.C = oldbit7;
    f.Z = *x == 0;
    f.HC = false;
    f.N = false;
  }

  // Rotate right
  else if (rot == DT_RR) {
    u8 oldbit0 = *x & 0x1;
    *x >>= 1;
    *x |= f.C << 7;
    f.C = oldbit0;
    f.Z = *x == 0;
    f.HC = 0;
    f.N = 0;
  }

  else if (rot == DT_SLA) {
    u8 bit7 = *x >> 7;
    f.C = bit7;
    *x <<= 1;
    *x &= 0xFE;

    f.Z = *x == 0;
    f.HC = 0;
    f.N = 0;
  }

  else if (rot == DT_SRA) {
    u8 oldbit7 = *x >> 7;
    f.C = *x & 0x1;
    *x >>= 1;
    *x |= (oldbit7 << 7);

    f.Z = *x == 0;
    f.HC = 0;
    f.N = 0;
  }

  else if (rot == DT_SWAP) {
    u8 old_lower4 = *x << 4;
    *x >>= 4; // move upper 4 to lower 4
    *x |= old_lower4;

    f.Z = *x == 0;
    f.C = 0;
    f.HC = 0;
    f.N = 0;
  }

  else if (rot == DT_SRL) {
    u8 oldbit0 = *x & 0x1;
    f.C = oldbit0;
    *x >>= 1;
    *x &= 0x7F;
    
    f.Z = *x == 0;
    f.C = oldbit0;
    f.HC = 0;
    f.N = 0;
  }
}

void Cpu::BIT_y_r(u8 n, Register * x) {
  if (x == NULL) {
    Address addr = hl();
    x = bus->GetAddressPointer(hl());
  }

  u8 val = (*x >> n) & 0x1;
  f.Z = !val;
  f.N = 0;
  f.HC = 1;
}

/* Reset bit n of register x */
void Cpu::RES_y_r(u8 n, Register * x) {
  if (x == NULL) {
    Tick(MEM_RW_CYCLES);
    x = bus->GetAddressPointer(hl());
  }
    
  u8 mask = (0xFF ^ (0x1 << n));
  *x &= mask;

  if (x == NULL) Tick(MEM_RW_CYCLES);
}

/* Reset bit y of register x */
void Cpu::SET_y_r(u8 n, Register * x) {
  if (x == NULL) {
    Tick(MEM_RW_CYCLES);
    x = bus->GetAddressPointer(hl());
  }
  
  u8 mask = 0x1 << n;
  *x |= mask;

  if (x == NULL) Tick(MEM_RW_CYCLES);
}
