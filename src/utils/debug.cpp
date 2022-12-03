
/* █▀▄ █▀▀ █▄▄ █░█ █▀▀    █░█ ▀█▀ █ █░░ █▀ */ 
/* █▄▀ ██▄ █▄█ █▄█ █▄█    █▄█ ░█░ █ █▄▄ ▄█ */ 

#include <stdio.h>
#include <iostream>
#include <iomanip>

#include "debug.h"
#include "../cpu/cpu.h"
#include "../ppu.h"
#include "../bus.h"

void Debugger::FetchState() {
  af = (int) cpu->af();
  bc = (int) cpu->bc();
  de = (int) cpu->de();
  hl = (int) cpu->hl();
  sp = (int) cpu->getSp();
  pc = (int) cpu->getPc();
  op = (int) cpu->getOp();
}

void Debugger::Regdump() {
  FetchState();
  std::cout << "Opcode 0x" << std::hex << std::setw(2) << std::setfill('0') << op <<
            ": " << cpu->opcode_names[op] << std::endl;
  std::cout << "af: " << std::hex << std::setw(4) << std::setfill('0') << af << std::endl;
  std::cout << "bc: " << std::hex << std::setw(4) << std::setfill('0') << bc << std::endl;
  std::cout << "de: " << std::hex << std::setw(4) << std::setfill('0') << de << std::endl;
  std::cout << "hl: " << std::hex << std::setw(4) << std::setfill('0') << hl << std::endl;
  std::cout << "sp: " << std::hex << std::setw(4) << std::setfill('0') << sp << std::endl;
  std::cout << "pc: " << std::hex << std::setw(4) << std::setfill('0') << pc << std::endl;
  std::cout << std::endl;
}

void Debugger::Stackdump(void) {
  std::cout << "=== STACK DUMP ===" << std::endl;
  std::cout << "Showing last 6 values on stack" << std::endl;

  for (int i = 0; i < 6; ++i) {
    if ((sp + i) >= 0xFFFE) {
      std::cout << "End of stack reached" << std::endl;
      return;
    }
    std::cout << "0x" << sp + i << ": " << std::hex << std::setw(2) << std::setfill('0') 
              << (int) bus->read(sp + i) << std::endl;
  }
}

/* Debugger::step
 * "ff n" to fast-forward n cycles
 * "bp n" to set an instruction breakpoint at pc n */
void Debugger::step() {
  // Update fastforward/breakpoint vars
  FetchState();
  if (ffSet) {
    --stepCycles;
    if (stepCycles == 0) {
      ffSet = false;
    } else {
      return;
    }
  } else if (bpSet) {
    if (pc == pcBreakpoint) {
      bpSet = false;
    } else {
      return;
    }
  }

  Regdump();

  // Parse user input for fastforward/bkpt
  bool stepSuccess = false;
  while (!stepSuccess) {
    // Get input
    std::string inputStream;
    std::string cmd, numStr;
    getline(std::cin, inputStream);
    cmd = inputStream.substr(0, inputStream.find(" "));
    int pos1 = inputStream.find(" ") + 1;
    int pos2 = inputStream.find(" ", pos1);
    numStr = inputStream.substr(pos1, pos2 - pos1);

    // Nothing entered: just step 1 instruction
    if (cmd == "" && numStr == "") {
      break;
    }
    
    unsigned int num;
    try {
      num = std::stoul(numStr, nullptr, 16);
    } catch (std::exception& e) {
      std::cout << "Debugger::step(): Invalid input (non-integer string)" << std::endl;
      continue;
    }

    // Parse input
    if (cmd == "bp") {
      pcBreakpoint = num;
      std::cout << "Breakpoint set at 0x" << std::hex << static_cast<int>(num) << std::endl;
      bpSet = true;
    } else if (cmd == "ff") {
      stepCycles = num;
      ffSet = true;
    } else if (cmd == "memview") {
      std::cout << "Value at mem address 0x" << numStr << " is " << (int) bus->read(num) << std::endl;
      continue;
    } else if (cmd == "q" || cmd == "quit"){
      std::cout << "Exit command received: quitting Amphy" << std::endl;
    } else if (cmd == "stack") {
      Stackdump();
      continue;
    } else {
      std::cout << "Debugger::step(): Invalid command (use ff or bp)"
            << std::endl;
      continue;
    }

    stepSuccess = true;
  }
}
