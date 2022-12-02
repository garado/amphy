#include <stdio.h>
#include <iostream>
#include <iomanip>

#include "cpu/cpu.h"
#include "utils/debug.h"
#include "ppu.h"
#include "win.h"
#include "bus.h"

//#define DEBUG

int main( int argc, char* args[] )
{
  Window* win = new Window;
  Bus* bus = new Bus;
  Cpu* cpu = new Cpu(bus);
  Ppu* ppu = new Ppu(bus);
  Debugger* debug = new Debugger(cpu);


  // Read ROM
  int8_t status = bus->CopyRom("gba/testrom-06-l.gb");  

  if (status == -1) {
    std::cout << "Exiting :(" << std::endl;
    return -1;
  }

  // //Start up SDL and create window
  // if( !win->init() ) {
  //   std::cout << "Failed to initialize" << std::endl;
  // } else {
  //   //Load media
  //   if( !win->loadMedia() ) {
  //     printf( "Failed to load media\n" );
  //   } else {
  //     win->applyImg();
  //   }
  // }

  // loop de loop
  int cycleCount = 0;
  int cyclesElapsed = 0;
  for (;;) {
    // Run CPU
    bool result;
    try {

      #ifdef DEBUG
      debug->step();
      #endif

      uint8_t cyclesElapsed = cpu->execute();
      cpu->AddCycles(cyclesElapsed);
      
      if (result == EXIT_FAILURE) {
        debug->Regdump();
        break;
      } else {
        debug->Regdump();
      }
    } catch (std::out_of_range &oor) {
      std::cout << "Fatal CPU error: exiting" << std::endl;
      debug->Regdump();
      break;
    }

    // Run PPU
    try {
      ppu->execute(cyclesElapsed);
    } catch (...) {
      std::cout << "Fatal PPU error: exiting" << std::endl;
      break;
    }
    // if (bus->read(0xFF44) == 0x01) {
    //   std::cout << "LY incremented, finally" << std::endl;
    //   std::cout << "instrs executed: " << yuh << std::endl;
    //   std::cout << "cycles: " << cycleCount << std::endl;
    //   cpu->Regdump();
    //   // break;
    // }

    // blarggs test - serial output
    // if (bus->read(0xFF02) == 0x81) {
    //   char c = bus->read(0xFF01);
    //   printf("%c", c);
    //   bus->write(0xFF02, 0);
    // }
  }

  // Free resources and close SDL
  win->close();

  return 0;
}
