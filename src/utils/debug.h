
#include <stdio.h>
#include <iostream>
#include <array>
#include <fstream>
#include <vector>

#ifndef DEBUG_H
#define DEBUG_H

class Cpu;
class Bus;

class Debugger
{
  private:
    Cpu* cpu;
    Bus* bus;
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
    void Stackdump(void);
    Debugger(Cpu* cpu_, Bus* bus_) {
      bus = bus_;
      cpu = cpu_;
    }
    ~Debugger();
};

#endif
