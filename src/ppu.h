
/* █▀▀ █▀█ ▄▀█ █▀█ █░█ █ █▀▀ █▀ */
/* █▄█ █▀▄ █▀█ █▀▀ █▀█ █ █▄▄ ▄█ */

#ifndef PPU_H
#define PPU_H

#include <SDL2/SDL.h>
#include "bus.h"
#include "cpu.h"
#include "platform/platform.h"

#define DOTS_OAM 80
#define DOTS_VBLANK_SCANLINE 456
#define DOTS_VBLANK_TOTAL 4560
#define DOTS_HBLANK 160
#define DOTS_PXTRANSFER 216
#define LY_AT_VBLANK_START 144

#define OAM_BYTES   40

#define NO_TRANSITION 0

#define OAM_YPOS 0
#define OAM_XPOS 1
#define OAM_TIDX 2
#define OAM_ATTR 3
#define OAM_ATTR_BGW_OVER_OBJ 7
#define OAM_ATTR_YFLIP 6
#define OAM_ATTR_XFLIP 5
#define OAM_ATTR_PALETTE 4


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

    // Window internal line counter (y coord)
    u16 wcnt = 0;
    bool doIncrementWcnt = false;
    bool doDrawWindow = false;

    u16 dotsSinceStateSwitch = 0;

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
    u8 * obp0;
    u8 * obp1;
    u8 * bgp;
    u8 * lyc;

    int ppuCyclesElapsed = 0;

    static Color gb_colors[4];
   
    std::vector<u16> spritesOnScanline;

    // PPU state machine
    void OAMScan(u8 *nextState);
    void PixelTransfer(u8 *nextState);
    void HBlank(u8 *nextState);
    void VBlank(u8 *nextState);

    void Px_RenderBgWindow(void);

    bool Px_RenderSprite(void);
    u8 Px_FindScanlineSprite(void);
    bool bgOverObj = false;

    void UpdateCycles(u8 state);

  public:
    void Init();
    void Execute(u8 cpuCyclesElapsed);
    int cnt = 144; // ???

    // Constructor & destructor
    Ppu(Bus* bus_, Display* disp_) {
      bus = bus_;
      disp = disp_;
    }

    friend class Debugger;
};

#endif
