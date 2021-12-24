//Using SDL and standard IO
#include <stdio.h>
#include <iostream>
#include <iomanip>

#include "cpu.h"
#include "win.h"

int main( int argc, char* args[] )
{
    // Window* win = new Window;
    Cpu* cpu = new Cpu;

    cpu->copyRom("gba/pkmn.gb");  

    // Infinite CPU loop
    for (;;) {
        bool result = cpu->execute();
        if (result == EXIT_FAILURE) break;
    }
    
    // // from lazyfoo (need to refactor later)
    // //Start up SDL and create window
    // if( !win->init() ) {
    //     std::cout << "Failed to initialize!" << std::endl;
    // } else {
    //     //Load media
    //     if( !win->loadMedia() ) {
    //         printf( "Failed to load media!\n" );
    //     } else {
    //         win->applyImg();
    //     }
    // }

    //Free resources and close SDL
    // win->close();

    return 0;
}