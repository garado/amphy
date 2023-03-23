#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <unistd.h>

#include "cpu/cpu.h"
#include "utils/debug.h"
#include "ppu.h"
#include "display/gba-sdl.h"
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

  // Parse flags
  // -g gbdoc mode
  // -d debug mode (step)
  int c;
  while ((c = getopt(argc, argv, ":gd")) != -1) {
    switch (c) {
      case 'g':
        cpu->gbdoc = true;
        break;
      case 'd':
        cpu->debug = true;
      default:
        break;
    }
  }

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

  // Start SDL and create new window
  if (disp->init() == EXIT_FAILURE) {
    std::cerr << "SDL: Failed to initialize" << std::endl;
  } else {
    // Load media
    if( disp->LoadMedia() == EXIT_FAILURE ) {
      std::cerr << "SDL: Failed to load media" << std::endl;
      disp->close();
    } else {
      disp->ApplyImg();
    }
  }

  // Main loop
  for (;;) {
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

  // win->close();

  return EXIT_SUCCESS;
}
