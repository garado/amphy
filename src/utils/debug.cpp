
#include <stdio.h>
#include <iostream>
#include <iomanip>

#include "debug.h"
#include "../cpu/cpu.h"
#include "../ppu.h"
#include "../bus.h"

void Debugger::fetchState() {
    af = (int) cpu->af();
    bc = (int) cpu->bc();
    de = (int) cpu->de();
    hl = (int) cpu->hl();
    sp = (int) cpu->getSp();
    pc = (int) cpu->getPc();
    op = (int) cpu->getOp();
}

void Debugger::regdump() {
    fetchState();
    std::cout << "af: " << std::hex << std::setw(4) << std::setfill('0') << af << "\t";
    std::cout << "sp: " << std::hex << std::setw(4) << std::setfill('0') << sp << std::endl;
    std::cout << "bc: " << std::hex << std::setw(4) << std::setfill('0') << bc << "\t";
    std::cout << "pc: " << std::hex << std::setw(4) << std::setfill('0') << pc << std::endl;
    std::cout << "de: " << std::hex << std::setw(4) << std::setfill('0') << de << "\t";
    std::cout << "op: " << std::hex << std::setw(2) << std::setfill('0') << op << std::endl;
    std::cout << "hl: " << std::hex << std::setw(4) << std::setfill('0') << hl;
    std::cout << std::endl << std::endl;
}

/* Debugger::step
 * "ff n" to fast-forward n cycles
 * "bp n" to set an instruction breakpoint at n 
 * */
void Debugger::step() {
    // Update fastforward/breakpoint vars
    fetchState();
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

    regdump();

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
        
        int num;
        try {
            num = stoi(numStr);
        } catch (std::exception& e) {
            std::cout << "Debugger::step(): Invalid input (non-integer string)"
                      << std::endl;
            continue;
        }

        // Parse input
        if (cmd == "bp") {
            pcBreakpoint = num;
            bpSet = true;
        } else if (cmd == "ff") {
            stepCycles = num;
            ffSet = true;
        } else {
            std::cout << "Debugger::step(): Invalid command (use ff or bp)"
                      << std::endl;
            continue;
        }

        stepSuccess = true;
    }
}