
#include <stdio.h>
#include <iostream>
#include <iomanip>

// Display and input stuff
#include "platform/platform.h"

#include "ppu/ppu.h"
#include "cpu/cpu.h"
#include "utils/debug.h"
#include "utils/utils.h"
#include "bus.h"
#include "defines.h"

int main( int argc, char* argv[] )
{
  Cpu* cpu;
  Debugger* debug;
  
  Display* disp = new Display;
  Bus* bus = new Bus;
  Ppu* ppu = new Ppu(bus, disp);
  cpu   = new Cpu(bus, debug, ppu);
  debug = new Debugger(cpu, bus, ppu);
  cpu->debugger = debug;
  debug->cpu = cpu;

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
  bus->write(LY, 0x90);
  bus->write(STAT, 0x81);
  bus->init();

  disp->Init();

  // Main loop
  while (!disp->amphy_quit) {

    // Handle events
    disp->HandleEvent();

    // Run CPU
    try {
      cpu->execute();
    } catch (...) {
      std::cout << __PRETTY_FUNCTION__ << ": Fatal CPU error: exiting" << std::endl;
      debug->RegdumpGBDoc();
      break;
    }

    // Run PPU
    uint8_t cycles_elapsed = cpu->get_last_cycles();
    bool ppu_exec_result = ppu->Execute(cycles_elapsed);
    if (ppu_exec_result == FAILURE) {
      std::cout << __PRETTY_FUNCTION__ << ": Fatal PPU error: exiting" << std::endl;
      break;
    }

    // Read serial output from Blargg's test roms
    if (!cpu->gbdoc) {
      if (bus->read(0xFF02) == 0x81) {
        char c = bus->read(0xFF01);
        std::cout << c << std::flush;
        bus->write(0xFF02, 0);
      }
    }

  }

  // Free resources and close SDL
  delete(cpu);
  delete(bus);
  delete(debug);
  delete(ppu);

  disp->Close();

  return EXIT_SUCCESS;
}
