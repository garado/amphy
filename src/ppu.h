
/* ░█▀▀░█▀▄░█▀█░█▀█░█░█░▀█▀░█▀▀░█▀▀*/ 
/* ░█░█░█▀▄░█▀█░█▀▀░█▀█░░█░░█░░░▀▀█*/
/* ░▀▀▀░▀░▀░▀░▀░▀░░░▀░▀░▀▀▀░▀▀▀░▀▀▀*/

#ifndef PPU_H
#define PPU_H

#include "cpu/cpu.h"
#include "bus.h"

#define OAM_SCAN_CYCLES     40
#define PX_TRANSFER_CYCLES  4
#define HBLANK_CYCLES       4 // hblank and vblank vals need to be verified
#define VBLANK_CYCLES       4

#define PX_TRANSFER_X_DURATION 240
#define PX_TRANSFER_Y_DURATION 160

#define HBLANK_X_DURATION   68
#define VBLANK_Y_DURATION   68

typedef enum PpuStates {
  OAM_SCAN,
  PIXEL_TRANSFER,
  HBLANK,
  VBLANK,
} PpuStates;

static const char* PpuStates_Str[] {
  "OAM_SCAN",
  "PIXEL_TRANSFER",
  "HBLANK",
  "VBLANK",
};

typedef enum PpuRegisters {
  CON = 0xFF40, // LCD control
  STAT, // LCD status
  SCY,  // Viewport Y pos
  SCX,  // Viewport X pos + 7
  LY,   // LCD y-coordinate
  LYC,  // LY compare (int generated when LY == LYC)
  WY,   // Window Y pos
  WX,   // Window X pos + 7
} PpuRegisters;


class Ppu
{
  private:
    // Cycles since the last time the PPU actually ran.
    int     cycles_since_last_exec;
    Bus*    bus;
    uint8_t Ppu_State = OAM_SCAN;

    uint16_t x = 0; // x coordinate for pixel transfer
    // idk how the gameboy does it?

    std::vector<uint8_t> oam;

    void OAMScan(void);
    void PixelTransfer(void);
    void HBlank(void);
    void VBlank(void);
    void UpdateCycles(uint8_t cycles_taken);
    bool CanExecute(void);

  public:
    bool Execute(uint8_t cpu_cycles_elapsed);

    // Constructor & destructor
    Ppu(Bus* bus_) {
      bus = bus_; 
    }
    ~Ppu();
};

#endif
