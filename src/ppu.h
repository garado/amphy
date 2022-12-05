
/* ░█▀▀░█▀▄░█▀█░█▀█░█░█░▀█▀░█▀▀░█▀▀*/ 
/* ░█░█░█▀▄░█▀█░█▀▀░█▀█░░█░░█░░░▀▀█*/
/* ░▀▀▀░▀░▀░▀░▀░▀░░░▀░▀░▀▀▀░▀▀▀░▀▀▀*/

#ifndef PPU_H
#define PPU_H

#include <SDL2/SDL.h>
#include "cpu/cpu.h"
#include "bus.h"
#include "win.h"

#define OAM_SCAN_CYCLES     40
#define PX_TRANSFER_CYCLES  4

#define HBLANK_CYCLES       4 // hblank and vblank vals need to be verified
#define VBLANK_CYCLES       4

#define PX_TRANSFER_X_DURATION 240
#define PX_TRANSFER_Y_DURATION 160

#define HBLANK_X_DURATION   68
#define VBLANK_Y_DURATION   68

#define OAM_BYTES   40

typedef enum OAM_Flags {
  CGB_PALETTE_NUM0,
  CGB_PALETTE_NUM1,
  CGB_PALETTE_NUM2,
  TILE_VRAM_BANK,
  PALETTE_NUM,
  X_FLIP,
  Y_FLIP,
  BG_OVER_OBJ,
} OAM_Flags;

typedef enum PpuStates {
  HBLANK,
  VBLANK,
  OAM_SCAN,
  PIXEL_TRANSFER,
} PpuStates;

// $FF41 STAT: LCD status
typedef enum StatBits {
  MODE_BIT0, // 0 = hblank, 1 = vblank
  MODE_BIT1, // 2 = oam, 3 = px transfer
  LYC_EQ_LY, // 0 == different, 1 == equal
  HBLANK_INT,
  VBLANK_INT,
  OAM_INT,
  LYC_LY_INT, // LYC = LY interrupt
} StatBits;

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
    Bus*    bus;
    Window* win;
    //uint8_t Ppu_State = OAM_SCAN;
    uint8_t Ppu_State = VBLANK; // blargg test rom init

    // Cycles since the last time the PPU actually ran.
    int       cycles_since_last_exec = 0;
    
    // x-coordinate for pixel transfer
    // not sure how the gameboy keeps track of this
    uint16_t  x = 0;

    std::vector<uint8_t> oam;

    void OAMScan(void);
    void PixelTransfer(void);
    void HBlank(void);
    void VBlank(void);
    void UpdateCycles(uint16_t cycles_taken);
    bool CanExecute(void);

  public:
    bool Execute(uint8_t cpu_cycles_elapsed);
    int cnt = 144;

    // Constructor & destructor
    Ppu(Bus* bus_, Window* win_) {
      bus = bus_;
      win = win_;
    }
};

#endif
