
/* █▀▀ █▀█ ▄▀█ █▀█ █░█ █ █▀▀ █▀ */
/* █▄█ █▀▄ █▀█ █▀▀ █▀█ █ █▄▄ ▄█ */

#ifndef PPU_H
#define PPU_H

#include <SDL2/SDL.h>
#include "../cpu/cpu.h"
#include "../bus.h"
#include "../platform/platform.h"

#define OAM_SCAN_CYCLES     40
#define PX_TRANSFER_CYCLES  4

#define HBLANK_CYCLES       4 // hblank and vblank vals need to be verified
#define VBLANK_CYCLES       4

#define PX_TRANSFER_X_DURATION 240
#define PX_TRANSFER_Y_DURATION 160

#define HBLANK_X_DURATION   68
#define VBLANK_Y_DURATION   68

#define OAM_BYTES   40

#define NO_TRANSITION 0

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
  HBLANK = 1,
  VBLANK,
  OAM_SCAN,
  PIXEL_TRANSFER,
} PpuStates;

static const char* PpuStates_Str[] {
  "NO_TRANSITION"
  "OAM_SCAN",
  "PIXEL_TRANSFER",
  "HBLANK",
  "VBLANK",
};

typedef enum PixelFetcherStates {
  GET_TILE = 1,
  GET_TILE_LOW,
  GET_TILE_HIGH,
  PUSH,
  IDLE,
} PixelFetcherStates;

static const char* PixelFetcherStates_Str[] {
  "NO_TRANSITION",
  "GET_TILE",
  "GET_TILE_LOW",
  "GET_TILE_HIGH",
  "PUSH",
  "IDLE",
};

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

class Ppu
{
  private:
    Bus*    bus;
    Display* disp;
    //uint8_t Ppu_State = OAM_SCAN;
    uint8_t Ppu_State = VBLANK; // blargg test rom init

    // Cycles since the last time the PPU actually ran.
    int cycles_since_last_exec = 0;
    
    // x coord for pixel transfer
    uint16_t x = 0;

    std::vector<uint8_t> oam;

    std::vector<uint8_t> oam_fifo;
    std::vector<uint8_t> bg_fifo;

    void OAMScan(void);
    void PixelTransfer(void);
    void HBlank(void);
    void VBlank(void);
    void UpdateCycles(uint16_t cycles_taken);
    bool CanExecute(void);

    static Color gb_colors[4];

    bool UseUnsignedAddressing(void);

  public:
    void calcTimeToNextInterrupt();
    uint16_t timeToNextInterrupt = 0;

    bool Execute(uint8_t cpu_cycles_elapsed);
    int cnt = 144;

    // Constructor & destructor
    Ppu(Bus* bus_, Display* disp_) {
      bus = bus_;
      disp = disp_;
    }
};

#endif
