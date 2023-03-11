
#include <stdio.h>
#include <iostream>
#include <array>
#include <fstream>
#include <vector>

#ifndef DEBUG_H
#define DEBUG_H

class Cpu;
class Bus;
class Ppu;

class Debugger
{
  private:
    Cpu* cpu;
    Bus* bus;
    Ppu* ppu;
    int af, bc, de, hl;
    int sp, pc;
    int op;

  public:
    int stepCycles;
    int pcBreakpoint;
    int memBreakpoint;
    bool ffSet, bpSet, mbpSet;
    void FetchState();

  public:
    void Regdump();
    void RegdumpGBDoc();
    void debugger();
    void step();
    void Stackdump(void);
    Debugger(Cpu* cpu_, Bus* bus_, Ppu* ppu_) {
      bus = bus_;
      cpu = cpu_;
      ppu = ppu_;
    }
};

#endif
