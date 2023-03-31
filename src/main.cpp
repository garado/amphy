
#include <stdio.h>
#include <iostream>
#include <iomanip>

#include "common.h"
#include "platform/platform.h"
#include "cpu.h"
#include "bus.h"
#include "ppu.h"
#include "utils.h"
#include "debug.h"

int main( int argc, char* argv[] )
{
  // TODO fix this disgusting bs
  Cpu* cpu;
  Bus* bus;
  Ppu* ppu;
  Display* disp;
  Debugger* debugger;

  disp = new Display;
  bus = new Bus;
  ppu = new Ppu(bus, disp);
  cpu = new Cpu(bus, ppu, debugger);
  debugger = new Debugger(cpu, bus, ppu);

  cpu->bus = bus;
  cpu->debugger = debugger;
  cpu->ppu = ppu;
  cpu->bus = bus;
  bus->cpu = cpu;

  ParseFlags(argc, argv, cpu);

  // Read ROM (default to test rom if nothing was given)
  bool bus_status;
  if (argc > 1) {
    bus_status = bus->CopyRom(argv[argc-1]);
  } else {
    std::cout << "No ROM provided! Exiting :(" << std::endl;
    return EXIT_SUCCESS;
  }

  if (bus_status == FAILURE) {
    std::cout << "Exiting :(" << std::endl;
    return EXIT_FAILURE;
  }

  // ly is initialized to 0x90 in blargg's tests?
  bus->Write(LY, 0x90);
  bus->Write(STAT, 0x81);
  bus->Init();

  disp->Init();

  while(!disp->amphy_quit) {

    disp->HandleEvent();

    try {
      cpu->Execute();
    } catch (...) {
      std::cout << __PRETTY_FUNCTION__ << ": Fatal CPU error: exiting" << std::endl;
      debugger->Regdump();
      return EXIT_FAILURE;
    }

    // Read serial output from Blargg's test roms
    if (bus->Read(0xFF02) == 0x81) {
      fprintf(stderr, "%c", bus->Read(0xFF01));
      bus->Write(0xFF02, 0);
    }
  }

  // Free resources and close SDL
  delete(cpu);
  delete(bus);
  delete(ppu);

  disp->Close();

  return EXIT_SUCCESS;
}
