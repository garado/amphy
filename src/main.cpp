//Using SDL and standard IO
#include <stdio.h>
#include <iostream>
#include <iomanip>

#include "cpu.h"
#include "win.h"

int main( int argc, char* args[] )
{
    Window* win = new Window;
    Cpu* cpu = new Cpu;

    cpu->copyRom("gba/pkmn.gb");  
    
    // std::cout << "Printing rom" << std::endl;
    // for (int i = 0; i < 0x3FFF; i++) {
    //     std::cout << std::hex << (int) cpu->mem[i] << std::endl;
    // }
    // std::cout << "Finished" << std::endl;

//    cpu->execute(instr);



    // std::ifstream infile("gba/pkmn.gba");
    // uint8_t instr;
    // if (!infile.is_open()) std::cout << "somethings wrong" << std::endl;
    // int max = 10;
    // while (infile.is_open()) {
    //     infile >> instr;
    //     std::cout << std::hex << (int) instr << std::endl;
    //     max--;
    //     if (max == 0) break;
    // }

    // from lazyfoo
    //Start up SDL and create window
    if( !win->init() ) {
        std::cout << "Failed to initialize!" << std::endl;
    } else {
        //Load media
        if( !win->loadMedia() ) {
            printf( "Failed to load media!\n" );
        } else {
            win->applyImg();
        }
    }

    //Free resources and close SDL
    win->close();

    return 0;
}