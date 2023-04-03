
/* █▀▀ █▀█ ▄▀█ █▀█ █░█ █ █▀▀ █▀ */
/* █▄█ █▀▄ █▀█ █▀▀ █▀█ █ █▄▄ ▄█ */

#ifndef PPU_H
#define PPU_H

#include <SDL2/SDL.h>
#include "bus.h"
#include "cpu.h"
#include "platform/platform.h"

#define VBLANK_TOTAL_CYCLES 226
#define PX_TRANSFER_X_DURATION 240
#define PX_TRANSFER_Y_DURATION 160
#define HBLANK_X_DURATION   68
#define VBLANK_Y_DURATION   68
#define OAM_BYTES   40

#define NO_TRANSITION 0

/* This mode numbering MUST be followed */
typedef enum PpuStates {
  // 0 means no_transition/invalid
  HBLANK = 1,
  VBLANK,
  OAM_SCAN,
  PIXEL_TRANSFER,
} PpuStates;

static const char* PpuStates_Str[] {
  "NO_TRANSITION",
  "HBLANK",
  "VBLANK",
  "OAM_SCAN",
  "PIXEL_TRANSFER",
};

static const u8 PPU_STATE_CYCLES[5] = {
  0,  // INVALID
  40, // OAM
  4,  // HBlank
  4,  // VBlank
  4,  // PxTransfer
};

class Ppu
{
  private:
    Bus*    bus;
    Display* disp;
    u8 ppuState = VBLANK; // not sure what it actually starts in

    // Cycles since the last time the PPU actually ran.
    int cyclesSinceLastExec = 0;
    
    // x coord for pixel transfer
    u16 x = 0;

    // Pointers to commonly used registers
    // saves me some keystrokes
    u8 * ly;
    u8 * wx;
    u8 * wy;
    u8 * scx;
    u8 * scy;
    u8 * stat;
    u8 * lcdc;
    u8 * intf;

    int ppuCyclesElapsed = 0;

    static Color gb_colors[4];
   
    std::vector<u16> spritesOnScanline;

    // PPU state machine
    void OAMScan(u8 *nextState);
    void PixelTransfer(u8 *nextState);
    void HBlank(u8 *nextState);
    void VBlank(u8 *nextState);

    bool Px_RenderSprite(void);
    u8 Px_FindScanlineSprite(void);
    void Px_RenderBackground(void);
    void Px_RenderWindow(void);

    // Helpers
    void UpdateCycles(u8 state);
    bool UseUnsignedAddressing(void);

  public:
    void Init();
    bool Execute(u8 cpuCyclesElapsed);
    int cnt = 144; // ???

    // Constructor & destructor
    Ppu(Bus* bus_, Display* disp_) {
      bus = bus_;
      disp = disp_;
    }

    friend class Debugger;
};

#endif
