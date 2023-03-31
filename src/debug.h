
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

class Debugger {
  private:
    Cpu* cpu;
    Bus* bus;
    Ppu* ppu;

    int bpOp = 0;
    int instrCount = 0;
    int stepCycles;
    int pcBreakpoint;
    int memBreakpoint;
    bool ffSet, bpSet, bpOpSet;

  private:
    void Help();
    void PrintCpuState();
    void PrintPpuState();

  public:
    void Regdump();
    void Step();

  public:
    Debugger(Cpu* cpu_, Bus* bus_, Ppu* ppu_) {
      bus = bus_;
      cpu = cpu_;
      ppu = ppu_;
    }
};

#endif
