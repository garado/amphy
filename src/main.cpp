//Using SDL and standard IO
#include <stdio.h>
#include <iostream>
#include <iomanip>

#include "cpu.h"
#include "ppu.h"
#include "win.h"
#include "bus.h"

int main( int argc, char* args[] )
{
    Window* win = new Window;
    Bus* bus = new Bus;

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
    bus->copyRom("gba/pkmn.gb");  

    // loop de loop
    int cycleCount = 0;
    int cyclesElapsed = 0;
    for (;;) {
        // run CPU
        bool result;
        result = cpu->execute();        
        if (result == EXIT_FAILURE) {
            break;
            cpu->regdump();
        }

        // run PPU
        cyclesElapsed = cpu->getCycles();
        ppu->execute(cyclesElapsed);

        cycleCount += cyclesElapsed;
    }

    // Free resources and close SDL
    win->close();

    return 0;
}