#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <unistd.h>

#include "cpu/cpu.h"
#include "utils/debug.h"
#include "ppu.h"
#include "win.h"
#include "bus.h"
#include "defines.h"

int main( int argc, char* argv[] )
{
  Cpu* cpu;
  Debugger* debug;
  
  Window* win = new Window;
  Bus* bus = new Bus;
  Ppu* ppu = new Ppu(bus, win);
  cpu   = new Cpu(bus, debug);
  debug = new Debugger(cpu, bus, ppu);

  // Parse flags
  // -g gbdoc mode
  int c;
  while ((c = getopt(argc, argv, ":g")) != -1) {
    switch (c) {
      case 'g':
        cpu->gbdoc = true;
        break;
      default:
        break;
    }
  }

  // Read ROM (default to test rom if nothing was given)
  bool bus_status;
  if (argc > 1) {
    bus_status = bus->CopyRom(argv[argc-1]);  
  } else {
    bus_status = bus->CopyRom("gba/testrom-06-ld.gb");  
  }

  if (bus_status == FAILURE) {
    std::cout << "Exiting :(" << std::endl;
    return EXIT_FAILURE;
  }

  // ly is initialized to 0x90 in blargg's tests?
  bus->write(LY, 0x90);
  bus->write(STAT, 0x81);

  // Start SDL and create new window
  // if (win->init() == EXIT_FAILURE) {
  //   std::cerr << "SDL: Failed to initialize" << std::endl;
  // } else {
  //   // Load media
  //   if( !win->LoadMedia() ) {
  //     std::cerr << "SDL: Failed to load media" << std::endl;
  //   } else {
  //     win->ApplyImg();
  //   }
  // }

  // Main loop
  for (;;) {
    // Run CPU
    if (cpu->gbdoc) {
      debug->RegdumpGBDoc();
    }

    try {
      cpu->execute();
    } catch (...) {
      std::cout << __PRETTY_FUNCTION__ << ": Fatal CPU error: exiting" << std::endl;
      debug->Regdump();
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
