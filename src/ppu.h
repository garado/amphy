
/* ░█▀▀░█▀▄░█▀█░█▀█░█░█░▀█▀░█▀▀░█▀▀*/ 
/* ░█░█░█▀▄░█▀█░█▀▀░█▀█░░█░░█░░░▀▀█*/
/* ░▀▀▀░▀░▀░▀░▀░▀░░░▀░▀░▀▀▀░▀▀▀░▀▀▀*/

#ifndef PPU_H
#define PPU_H

#include "cpu/cpu.h"
#include "bus.h"

class Ppu
{
  private:
    Bus* bus;

  public:
    bool Execute();

    // Constructor & destructor
    Ppu(Bus* bus_) {
      bus = bus_; 
    }
    ~Ppu();
};

#endif
