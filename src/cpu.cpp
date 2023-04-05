
/* █▀▀ █▀█ █░█ */
/* █▄▄ █▀▀ █▄█ */

#include "common.h"
#include "debug.h"
#include "cpu.h"
#include "ppu.h"
#include "bus.h"

#include <cstdio>

void Cpu::Init() {
  divPtr = bus->GetAddressPointer(DIV);
  joypPtr = bus->GetAddressPointer(JOYP);
  intf = bus->GetAddressPointer(INTF);
  inte = bus->GetAddressPointer(INTE);
  tima = bus->GetAddressPointer(TIMA);
  tma = bus->GetAddressPointer(TMA);
  tac = bus->GetAddressPointer(TAC);
}

/* @Function Cpu::Execute
 * @brief Execute one instruction. */
void Cpu::Execute() {
  HandleInterrupt();

  switch (cpuState) {
    case CPU_NORMAL:
      RunInstruction();
      break;

    case CPU_EI:
      RunInstruction();
      ime = true;
      if (cpuState == CPU_EI) {
        cpuState = CPU_NORMAL;
      }
      break;

    case CPU_HALT_IME_SET:
      if (gbdoc || step) debugger->Step();
      Tick(NOP_CYCLES);
      break;

    case CPU_HALT_IME_NOT_SET:
      if (gbdoc || step) debugger->Step();
      Tick(NOP_CYCLES);
      break;

    // Byte after halt is read twice
    case CPU_HALT_BUG:
      RunInstruction();
      cpuState = CPU_NORMAL;
      break;

    case CPU_STOP:
      break;

    default: break;
  }
}

/* @Function Cpu::RunInstruction */
void Cpu::RunInstruction() {
  // Halt bug: Instruction after HALT is read twice
  if (cpuState == CPU_HALT_BUG) {
    if (step) debugger->Step();
    if (gbdoc) debugger->Regdump();
    op = MemReadRaw(pc);
    if (op == 0xCB) {
      Decode16BitOpcode();
    } else {
      Decode8BitOpcode();
    }
  } else {
    op = MemReadRaw(pc);
    if (step) debugger->Step();
    if (gbdoc) debugger->Regdump();
    op = MemRead_u8(&pc);
    if (op == 0xCB) {
      Decode16BitOpcode();
    } else {
      Decode8BitOpcode();
    }
  }
}

/* @Function Cpu::SetFlags 
 * @brief Sets flag register to 8-bit number. 
 * The lower 4 bits of argument should be 0s.
 * The upper 4 bits are the Z, N, HC, and C
 * flags (from msb to lsb). */
void Cpu::SetFlags(u8 flags) {
  flags >>= 4;
  f.C  = flags & 0x1;
  f.HC = (flags >> 1) & 1;
  f.N  = (flags >> 2) & 1;
  f.Z  = (flags >> 3) & 1;
}

void Cpu::SetFlags(bool z, bool n, bool hc, bool c) {
  f.Z = z;
  f.N = n;
  f.HC = hc;
  f.C = c;
}

/* @Function Cpu::GetFlagsAsInt
 * @brief Returns 8-bit flag register.
 *    The lower 4 bits are 0.
 *    The upper 4 bits are the Z, N, HC, and C flags 
 *    (from msb to lsb). */
u8 const Cpu::GetFlagsAsInt()
{
  return (f.Z << 7) | (f.N << 6) | (f.HC << 5) | (f.C << 4);
}

/* █ █▄░█ ▀█▀ █▀▀ █▀█ █▀▀ ▄▀█ █▀▀ █▀▀ */
/* █ █░▀█ ░█░ ██▄ █▀▄ █▀░ █▀█ █▄▄ ██▄ */

/* @Function Cpu::Tick
 * @brief Ticks all other subsystems. */
void Cpu::Tick(u8 cycles) {
  RunTimer(cycles);
  ppu->Execute(cycles);

  if (doDMATransfer) DMA_Transfer();
}

/* @Function Cpu::DMA_Transfer 
 * @brief Called from Tick() function . Emulates DMA transfer 
*   of sprite data from WRAM to OAM. */
void Cpu::DMA_Transfer() {
  u8 val = bus->Read(dmaAddr++);
  // printf("dma transfer: %02X\n", val);
  bus->Write(OAM_START + dmaByteCnt++, val);
  if (dmaByteCnt == 160) doDMATransfer = false;
}

/* Memory read/write wrappers.
 * To get more accurate instruction timing, ticks PPU
 * on every memory read/write. Also increments the address
 * to be read/written from. */
u8 Cpu::MemReadRaw(Address addr) {
  return bus->Read(addr);
}

u8 Cpu::MemRead_u8(Address * addr) {
  Tick(MEM_RW_CYCLES);
  return MemReadRaw((*addr)++);
}

u16 Cpu::MemRead_u16(Address * addr) {
  Tick(MEM_RW_CYCLES);
  u8 lsb = MemReadRaw((*addr)++);
  Tick(MEM_RW_CYCLES);
  u8 msb = MemReadRaw((*addr)++);
  return (msb << 8) | lsb;
}

void Cpu::MemWriteRaw(Address addr, u8 value) {
  bus->Write(addr, value);
}

void Cpu::MemWrite_u8(Address * addr, u8 value) {
  MemWriteRaw((*addr)++, value);
  Tick(MEM_RW_CYCLES);
}

void Cpu::MemWrite_u16(Address * addr, u16 value) {
  MemWrite_u8(addr, value & 0xFF);
  MemWrite_u8(addr, value >> 8);
}

void Cpu::Push_u8(u8 value) {
  --sp;
  Tick(ALU_CYCLES);
  MemWriteRaw(sp, value);
  Tick(MEM_RW_CYCLES);
}

void Cpu::Push_u16(u16 value) {
  Push_u8(value >> 8);
  Push_u8(value & 0xFF);
}

/* █▀▀ █░░ ▄▀█ █▀▀ █▀ */
/* █▀░ █▄▄ █▀█ █▄█ ▄█ */

/* ADDITION */
/* C set for 16-bit addition when there is ovflw out of 16th bit. */
void Cpu::SetFlags_16bitAdd_C(u16 x, u16 y)
{
  f.C = ((uint32_t) x + (uint32_t) y) > 0xFFFF;
}

/* HC set for 16-bit addition on carry from bit 11 to 12. */
void Cpu::SetFlags_16bitAdd_HC(u16 x, u16 y)
{
  u16 sum = (x & 0xFFF) + (y & 0xFFF);
  f.HC = (sum & 0x1000) == 0x1000;
}

/* 8bit C set when addition overflows out of the 4th bit */
void Cpu::SetFlags_8bitAdd_C(u8 x, u8 y)
{
  f.C = (int) (x + y) > 0xFF;
}

/* 8bit HC set on carry from 3rd to 4th bit. */
void Cpu::SetFlags_8bitAdd_HC(u8 x, u8 y)
{
  u16 sum = (x & 0xF) + (y & 0xF);
  f.HC = (sum & 0x10) == 0x10;
}

/* SUBTRACTION */
/* C set for 16bit sub when a borrow is required from the MSB. */
void Cpu::SetFlags_16bitSub_C(u16 x, u16 y)
{
  f.C = x > y;
}

/* HC set for 16bit sub when TODO */
void Cpu::SetFlags_16bitSub_HC(u16 x, u16 y)
{
  f.C = (x & 0xF) + (y & 0xF) > 0xF;
}

/* C set for 8bit sub when TODO */
void Cpu::SetFlags_8bitSub_C(u8 x, u8 y)
{
  f.C = (int) (x - y) < 0;
}

/* HC for 8bit sub set when TODO */
void Cpu::SetFlags_8bitSub_HC(u8 x, u8 y)
{
  f.HC = ((x & 0xF) - (y & 0xF)) & 0x10;
}

/* █▀█ █▀█ █▀▀ █▀█ █▀▄ █▀▀ █▀ */
/* █▄█ █▀▀ █▄▄ █▄█ █▄▀ ██▄ ▄█ */

/* @Function Cpu::Decode8BitOpcode
 * @brief Algorithmic opcode decoding.
 * I don't know if it's faster than a table of function pointers
 * or a beeg switch/case, but I wanted to try it out.
 * https://gb-archive.github.io/salvage/decoding_gbz80_opcodes/Decoding%20Gamboy%20Z80%20Opcodes.html */
void Cpu::Decode8BitOpcode()
{
  u8 x = op >> 6;            // Bit 6-7
  u8 y = (op >> 3) & 0b111;  // Bit 3-5
  u8 z = op & 0b111;         // Bit 0-2
  u8 p = y >> 1;   // Bit 4-5
  u8 q = y & 0b1;  // Bit 3

  if (x == 0) {

    // Relative jumps and assorted ops
    if (z == 0) {
      switch (y) {
        case 0: break; // NOP: do nothing
        case 1: LD_atNN_SP(); break;
        case 2: STOP(); break;
        case 3: JR_s8(); break;
        case 4: 
        case 5:
        case 6:
        case 7: JR_cc_s8(DT_cc[y-4], y & 0x1); break;
        default: break;
      }
    }

    // 16-bit load immediate/add
    else if (z == 1)  {
      if (q == 0) LD_rp_NN(p);
      if (q == 1) ADD_HL_rp(p);
    }
   
    // Indirect loading
    else if (z == 2) {
      if (q == 0) {
        switch (p) {
          case 0: LD_atBC_A(); break;
          case 1: LD_atDE_A(); break;
          case 2: LD_atHLi_A(); break;
          case 3: LD_atHLd_A(); break;
          default: break;
        }
      }

      else if (q == 1) {
        switch (p) {
          case 0: LD_A_atBC(); break;
          case 1: LD_A_atDE(); break;
          case 2: LD_A_atHLi(); break;
          case 3: LD_A_atHLd(); break;
          default: break;
        }
      }
    }
  
    // 16-bit INC/DEC
    else if (z == 3) {
      if (q == 0) INC_rp(p);
      if (q == 1) DEC_rp(p);
    }
   
    // 8-bit INC
    else if (z == 4) INC_r(DT_r[y]);

    // 8-bit DEC
    else if (z == 5) DEC_r(DT_r[y]);

    // 8-bit load immediate
    else if (z == 6) LD_r_d8(DT_r[y]);

    // Assorted operations on acc/flags
    else if (z == 7) {
      switch (y) {
        case 0: RLCA(); break;
        case 1: RRCA(); break;
        case 2: RLA(); break;
        case 3: RRA(); break;
        case 4: DAA(); break;
        case 5: CPL(); break;
        case 6: SCF(); break;
        case 7: CCF(); break;
        default: break;
      }
    }
  }

  // 8-bit loading
  else if (x == 1) {
    if (z == 6 && y == 6) {
      HALT();
    } else {
      LD_r1_r2(DT_r[y], DT_r[z]);
    }
  }

  // ALU operations
  else if (x == 2) ALU_r(DT_alu[y], DT_r[z]);

  else if (x == 3) {
    // Conditional return, mem-mapped register loads,
    // and stack operations
    if (z == 0) {
      switch (y) {
        case 0:
        case 1:
        case 2:
        case 3:
          RET_cc(DT_cc[y], y & 0x1);
          break;
        case 4: LD_a8_A();    break;
        case 5: ADD_SP_s8();  break;
        case 6: LD_A_a8();    break;
        case 7: LD_HL_SPs8(); break;
        default: break;
      }
    }

    // POP & various ops
    else if (z == 1) {
      if (q == 0) {
        POP_rp2(p);
      } else {
        if (p == 0) RET();
        else if (p == 1) RETI();
        else if (p == 2) JP_HL();
        else if (p == 3) LD_SP_HL();
      }
    }

    // Conditional jumps
    else if (z == 2) {
      switch(y) {
        case 0:
        case 1:
        case 2:
        case 3:
          JP_cc_d16(DT_cc[y], y & 0x1);
          break;
        case 4: LD_atC_A();   break;
        case 5: LD_a16_A(); break;
        case 6: LD_A_atC();   break;
        case 7: LD_A_a16(); break;
        default: break;
      }
    }

    // Assorted operations
    else if (z == 3) {
      if (y == 0) JP_nn();
      else if (y == 6) DI();
      else if (y == 7) EI();
    }

    // Conditional call
    else if (z == 4) {
      CALL_cc_a16(DT_cc[y], y & 0x1);
    }

    // PUSH & various ops
    else if (z == 5) {
      if (q == 0) PUSH_rp2(p);
      else CALL_a16();
    }

    // Operate on accumulator and immediate operand
    else if (z == 6) ALU_n(DT_alu[y]);

    // Restart
    else if (z == 7) RST_y(y * 8);
  }
}

/* @Function Cpu::Decode16BitOpcode
 * @brief Algorithmic opcode decoding. */
void Cpu::Decode16BitOpcode()
{
  op = MemRead_u8(&pc);

  u8 x = op >> 6;            // Bit 6-7
  u8 y = (op >> 3) & 0b111;  // Bit 3-5
  u8 z = op & 0b111;         // Bit 0-2
  u8 p = y >> 1;   // Bit 4-5
  u8 q = y & 0b1;  // Bit 3

  // Roll/shift register or mem location
  if (x == 0) ROT_y_z(DT_rot[y], DT_r[z]);
  else if (x == 1) BIT_y_r(y, DT_r[z]);
  else if (x == 2) RES_y_r(y, DT_r[z]);
  else if (x == 3) SET_y_r(y, DT_r[z]);
}

/* @Function Cpu::HandleInterrupt
 * @brief Check for and respond to interrupts 
 *    For an interrupt to occur: 
 *      - an IRQ must be received
 *      - the interrupt must be enabled
 *      - the IME flag must be set */
void Cpu::HandleInterrupt()
{
  u8 ie = MemReadRaw(INTE);
  u8 irq = MemReadRaw(INTF);

  u8 intrPending = ie & irq;

  // Do nothing if no interrupts pending
  if (!intrPending) return;

  // Unhalt
  if (intrPending) {
    if (cpuState == CPU_HALT_IME_NOT_SET) {
      cpuState = CPU_HALT_BUG;
    } else if (cpuState == CPU_HALT_IME_SET) { // untested
      cpuState = CPU_NORMAL;
    }
  }

  // Now check for and respond to interrupts
  // The ordering in the Intr_Bits/Intr_Addr arrays 
  // reflect interrupt priority

  if (!ime) return;

  for (u8 i = 0; i < INTR_TYPES; ++i) {
    u8 ieCurr  = BIT_TEST(ie, Intr_Bits[i]);
    u8 irqCurr = BIT_TEST(irq, Intr_Bits[i]);

    if (ieCurr & irqCurr) {
      Push_u16(pc);
      pc = Intr_Addr[i];
      prevIme = ime;
      ime = false;
      *intf = BIT_CLEAR(INTF, Intr_Bits[i]);
      return;
    }
  }
}

/* @Function Cpu::RunTimer
 * @brief Ticks system clock and requests timer interrupt when
 * necessary.
 *
 * Generally this is called within the Tick() function (which
 * is then called when there is an instruction with a memory
 * read/write or alu op). when cpu is halted, this is called
 * manually. */
void Cpu::RunTimer(u8 cycles)
{
  sysclk += cycles;
  *divPtr = sysclk >> 8;

  // TIMA is incremented at the frequency specified by TAC.
  // When the value > 0xFF (overflows), it is reset to the value
  // specified in TMA and an interrupt is requested.
  // Don't do anything if timer not enabled
  if (!TAC_ENABLE_MASK(*tac)) {
    oldSysclk = sysclk;
  }

  if (doneTMAreload) doneTMAreload = false;

  // TIMA reload
  if (doTMAreload) {
    tmaReload -= cycles;
    if (tmaReload == 0) {
      *tima = *tma;
      doTMAreload = false;

      // helps prevent writes to tima while tima is
      // reloading
      doneTMAreload = true;
    }
  }

  // If tac_select has value TAC_1024, then it updates every
  // 1024 clock cycles. log_2(1024) = 10. We can tell if 1024
  // cycles have passed by checking if the 10th bit of sysclk
  // changes state.
  u16 tac_mask = 0;
  switch(TAC_SELECT_MASK(*tac)) {
    case TAC_1024:  tac_mask = 0x400; break;
    case TAC_16:    tac_mask = 0x10;  break;
    case TAC_64:    tac_mask = 0x40;  break;
    case TAC_256:   tac_mask = 0x100; break;
    default: break;
  }

  // XOR returns 1 if bits are different (state change). Then
  // we mask with tac_mask to check if a specific bit changes state.
  if ((oldSysclk ^ sysclk) & tac_mask) {
    // Request interrupt and reset TIMA to value in TMA if
    // overflow occurs. Increment normally otherwise.
    if (*tima == 0xFF) {

      // After overflow, TIMA contains 0 for 4 m-cycles before
      // being reloaded with value from TMA register
      doTMAreload = true;
      tmaReload = TMA_RELOAD;
      *tima = 0;
      *intf = BIT_SET(*intf, INTF_TMR_IRQ);
    } else {
      (*tima)++;
    }
  }

  oldSysclk = sysclk;
}

static const char* keytype_str[2] {
  "KEYTYPE_DIRECTION",
  "KEYTYPE_ACTION",
};

static const char* key_str[4] {
  "KEY_R_A",
  "KEY_L_B",
  "KEY_UP_SEL",
  "KEY_DW_START",
};

/* @param type  keytype_dir or keytype_act
 * @param key   number 0-3 indicating key type
 * @brief 0 == pressed, so clear bit from corresponding key vector
 *    Also set interrupt flag when necessary */
void Cpu::Key_Down(KeyType type, Keys key) {
  u8 * vec = (type == KEYTYPE_DIR) ? &keyvec_dir : &keyvec_act;

  printf("Cpu: Key_Down: %s %s\n", keytype_str[type], key_str[key]);

  // Interrupt on high to low if corresponding keytype bit is selected
  u8 selBit, curType;
  selBit  = BIT_TEST(*joypPtr, JOYP_SEL_ACTION);

  if (selBit == JOYP_SEL_ACT_VAL) {
    curType = KEYTYPE_ACT;
  } else if (selBit == JOYP_SEL_DIR_VAL) {
    curType = KEYTYPE_DIR;
  }

  if (curType == type && BIT_GET(*vec, key) != 0) {
    *intf = BIT_SET(*intf, INTF_JOYP_IRQ);
  }

  *vec = BIT_CLEAR(*vec, key);
}

/* @brief 1 == unpressed, so set bit from corresponding key vector
 * @param type keytype_dir or keytype_act
 * @param key  number 0-3 indicating key type and also bit position */
void Cpu::Key_Up(KeyType type, Keys key) {
  u8 * vec = (type == KEYTYPE_DIR) ? &keyvec_dir : &keyvec_act;
  *vec = BIT_SET(*vec, key);
}
