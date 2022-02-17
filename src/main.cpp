//Using SDL and standard IO
#include <stdio.h>
#include <iostream>
#include <iomanip>

#include "cpu/cpu.h"
#include "utils/debug.h"
#include "ppu.h"
#include "win.h"
#include "bus.h"

int main( int argc, char* args[] )
{
    Window* win = new Window;
    Bus* bus = new Bus;
    bus->copyRom("gba/testrom-06-ld.gb");  

    // //Start up SDL and create window
    // if( !win->init() ) {
    //     std::cout << "Failed to initialize" << std::endl;
    // } else {
    //     //Load media
    //     if( !win->loadMedia() ) {
    //         printf( "Failed to load media\n" );
    //     } else {
    //         win->applyImg();
    //     }
    // }

    Cpu* cpu = new Cpu(bus);
    Ppu* ppu = new Ppu(bus);
    Debugger* debug = new Debugger(cpu);

    // loop de loop
    int cycleCount = 0;
    int cyclesElapsed = 0;
    for (;;) {
        // run CPU
        bool result;
        try {
            debug->step();

            uint8_t cyclesElapsed = cpu->execute();
            cpu->addCycles(cyclesElapsed);
            
            if (result == EXIT_FAILURE) {
                debug->regdump();
                break;
            } else {
                //cpu->regdump();
                //std::cout << std::endl << std::endl;
            }
        } catch (std::out_of_range &oor) {
            std::cout << "somethin fucked up she outta range... regdumping" << std::endl;
            debug->regdump();
            break;
        }

        // run PPU
        ppu->execute(cyclesElapsed);
        // if (bus->read(0xFF44) == 0x01) {
        //     std::cout << "LY incremented, finally" << std::endl;
        //     std::cout << "instrs executed: " << yuh << std::endl;
        //     std::cout << "cycles: " << cycleCount << std::endl;
        //     cpu->regdump();
        //     // break;
        // }

        // blarggs test - serial output
        if (bus->read(0xFF02) == 0x81) {
            char c = bus->read(0xFF01);
            printf("%c", c);
            bus->write(0xFF02, 0);
        }
    }

    // Free resources and close SDL
    win->close();

    return 0;
}