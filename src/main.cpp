
#include <stdio.h>

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
  disp->cpu = cpu;

  ParseFlags(argc, argv, cpu);

  // Read ROM (default to test rom if nothing was given)
  bool bus_status;
  if (argc > 1) {
    bus_status = bus->CopyRom(argv[argc-1]);
  } else {
    printf("No ROM provided! Exiting :(\n");
    return EXIT_SUCCESS;
  }

  if (bus_status == FAILURE) {
    printf("Exiting :(\n");
    return EXIT_FAILURE;
  }

  bus->Init();
  cpu->Init();
  ppu->Init();
  disp->Init();

  while(!disp->amphy_quit) {

    // disp->HandleEvent();

    try {
      cpu->Execute();
    } catch (...) {
      printf("Fatal CPU error: exiting\n");
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
