
#include <stdio.h>
#include <iostream>
#include <array>
#include <fstream>
#include <vector>

#ifndef DEBUG_H
#define DEBUG_H

class Cpu;

class Debugger
{
  private:
    Cpu* cpu;
    int af, bc, de, hl;
    int sp, pc;
    int op;
    int stepCycles;
    int pcBreakpoint;
    bool ffSet, bpSet;
    void FetchState();

  public:
    void Regdump();
    void debugger();
    void step();
    Debugger(Cpu* cpu_) {
      cpu = cpu_;
    }
    ~Debugger();
};

#endif
