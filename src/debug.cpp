
/* █▀▄ █▀▀ █▄▄ █░█ █▀▀    █░█ ▀█▀ █ █░░ █▀ */ 
/* █▄▀ ██▄ █▄█ █▄█ █▄█    █▄█ ░█░ █ █▄▄ ▄█ */ 

#include <stdio.h>
#include <iostream>
#include <iomanip>

#include "debug.h"
#include "cpu.h"
#include "ppu.h"
#include "bus.h"

void Debugger::Help() {
  printf("=== AMPHY DEBUGGER ===\n" \
      "Options: \n" \
      "   bp <n>        Set breakpoint at instruction address n\n" \
      "   bpop <n>      Set breakpoint at opcode n (16bit ops not supported)\n" \
      "   ff <n>        Fast-forward n cycles\n" \
      "   mv <n>        View memory at address n\n" \
      "   s <cpu|ppu>   View cpu/ppu state information\n"
      );
}

// Dumps in this format:
// A:00 F:11 B:22 C:33 D:44 E:55 H:66 L:77 SP:8888 PC:9999 PCMEM:AA,BB,CC,DD
// For usage with GameboyDoctor
void Debugger::Regdump() {
  printf("A:%02X ", cpu->a);
  
  if (cpu->gbdoc) {
    printf("F:%02X ", cpu->GetFlagsAsInt());
  } else {
    printf("F:%c%c%c%c ", 
      cpu->f.C ? 'C' : '-',
      cpu->f.HC ? 'H' : '-',
      cpu->f.N ? 'N' : '-',
      cpu->f.Z ? 'Z' : '-');
  }

  printf("B:%02X C:%02X D:%02X E:%02X H:%02X L:%02X ",
      cpu->b, cpu->c, cpu->d, cpu->e, cpu->h, cpu->l);

  printf("SP:%04X PC:%04X ",
      cpu->sp, cpu->pc);

  printf("PCMEM:%02X,%02X,%02X,%02X",
      bus->Read(cpu->pc),
      bus->Read(cpu->pc+1),
      bus->Read(cpu->pc+2),
      bus->Read(cpu->pc+3));


  if (!cpu->gbdoc) {
    printf(" %02X %s",
        cpu->op,
        cpu->opcode_8bit_names[cpu->op]); 
  }

  if (!cpu->gbdoc) {
    printf(" IC:%d", instrCount);
  }

  printf("\n");
}

/* @Function Debugger::step */
void Debugger::Step() {
  instrCount++;

  if (bpOpSet) {
    if (cpu->op == bpOp) {
      bpOpSet = false;
    } else {
      return;
    }
  }

  if (ffSet) {
    --stepCycles;
    if (stepCycles == 0) {
      ffSet = false;
    } else {
      return;
    }
  }

  if (bpSet) {
    if (cpu->pc == pcBreakpoint) {
      bpSet = false;
    } else {
      return;
    }
  }

  Regdump();

  // Parse user input
  bool stepSuccess = false;
  while (!stepSuccess) {
    // Get input
    std::string inputStream;
    std::string cmd, arg1;
    getline(std::cin, inputStream);
    cmd = inputStream.substr(0, inputStream.find(" "));
    int pos1 = inputStream.find(" ") + 1;
    int pos2 = inputStream.find(" ", pos1);
    arg1 = inputStream.substr(pos1, pos2 - pos1);

    // Nothing entered: just step 1 instruction
    if (cmd == "" && arg1 == "") {
      break;
    }

    if (cmd == "help") {
      Help();
      continue;
    }

    if (cmd == "state") {
      if (arg1 == "cpu") {
        PrintCpuState();
      } else if (arg1 == "ppu") {
        PrintPpuState();
      }
      continue;
    }
    
    unsigned int num;
    try {
      num = std::stoul(arg1, nullptr, 16);
    } catch (std::exception& e) {
      std::cout << "Debugger::step(): Invalid input (non-integer string)" << std::endl;
      continue;
    }

    // Parse input
    if (cmd == "bp") {
      printf("Breakpoint set at 0x%04X\n", num);
      pcBreakpoint = num;
      bpSet = true;
    }
    
    else if (cmd == "ff") {
      printf("Fast-forwarding %d cycles\n", num);
      stepCycles = num;
      ffSet = true;
    }

    else if (cmd == "mv") {
      printf("[%04X] = %02X\n", num, bus->Read(num));
      continue;
    }

    else if (cmd == "bpop") {
      printf("Setting breakpoint at opcode %02X (%s)\n",
          num, cpu->opcode_8bit_names[num]);
      bpOp = num;
      bpOpSet = true;
    }

    else {
      printf("Debugger::Step(): Invalid command ('h' to show commands)\n");
      continue;
    }

    stepSuccess = true;
  }
}

void Debugger::PrintCpuState() {
  printf("=== CPU STATE ===\n" \
      "State: %s\n" \
      "IME: %s\n"   \
      "IE: %02X%s%s%s%s%s \n"  \
      "IF: %02X%s%s%s%s%s \n", \
      Cpu_StatesStr[cpu->cpuState],
      (cpu->ime) ? "enabled" : "disabled",
      bus->Read(INTE),
      bus->BitTest(INTE, 0) ? " VBLANK" : "",
      bus->BitTest(INTE, 1) ? " STAT" : "",
      bus->BitTest(INTE, 2) ? " TMR" : "",
      bus->BitTest(INTE, 3) ? " SRL" : "",
      bus->BitTest(INTE, 4) ? " JOYP" : "",
      bus->Read(INTF),
      bus->BitTest(INTF, 0) ? " VBLANK" : "",
      bus->BitTest(INTF, 1) ? " STAT" : "",
      bus->BitTest(INTF, 2) ? " TMR" : "",
      bus->BitTest(INTF, 3) ? " SRL" : "",
      bus->BitTest(INTF, 4) ? " JOYP" : ""
      );
}

void Debugger::PrintPpuState() {
  printf("=== PPU STATE ===\n" \
      "State: %s\n"     \
      "x, ly: %d %d\n"  \
      "cnt: %d\n",      \
      PpuStates_Str[ppu->ppuState],
      ppu->x,
      bus->Read(LY),
      ppu->cnt
      );
}
