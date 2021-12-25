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
    bus->copyRom("gba/testrom-06-ld r,r.gb");  

    // loop de loop
    int cycleCount = 0;
    int yuh = 0;
    int cyclesElapsed = 0;
    for (;;) {
        ++yuh;
        // run CPU
        bool result;
        try {
            result = cpu->execute();        
            if (result == EXIT_FAILURE) {
                break;
                cpu->regdump();
            } else {
                cpu->regdump();
                std::cout << std::endl << std::endl;
            }
        } catch (std::out_of_range &oor) {
            std::cout << "somethin fucked up she outta range" << std::endl;
            cpu->regdump();
            break;
        }

        // run PPU
        cyclesElapsed = cpu->getCycles();
        ppu->execute(cyclesElapsed);

        cycleCount += cyclesElapsed;

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