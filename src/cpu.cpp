
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "cpu.h"

#define CART_TYPE 0x147

/* Cpu::copyRom
 * Copies ROM from .gb file to memory.
 * MBC unimplemented. suuuuper basic.
 */
void Cpu::copyRom(std::string fname) {
    // Open rom for reading
    std::ifstream infile("gba/tetris.gb", std::ios::binary);
    if (!infile.is_open()) {
        std::cout << "ROM could not be opened" << std::endl;
    } else {
        std::cout << "ROM opened successfully" << std::endl;
    }

    // Copy rom into vector
    std::vector<uint8_t> rom(
         (std::istreambuf_iterator<char>(infile)),
         (std::istreambuf_iterator<char>()));
    infile.close();

    // Check and handle cartridge types
    uint8_t ctype = rom.at(CART_TYPE);
    switch (ctype) {
        case 0x00:  mem = rom;
        default:    break;
    }
}

void Cpu::execute(uint8_t opcode) {
    std::cout << "test:" << (int) opcode << std::endl;
};