
/* █▀▀ █▀█ ▄▀█ █▀█ █░█ █ █▀▀ █▀ */
/* █▄█ █▀▄ █▀█ █▀▀ █▀█ █ █▄▄ ▄█ */

#ifndef PPU_H
#define PPU_H

#include <SDL2/SDL.h>
#include "cpu/cpu.h"
#include "bus.h"
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
  "OAM_SCAN",
  "PIXEL_TRANSFER",
  "HBLANK",
  "VBLANK",
};

static const uint8_t PPU_STATE_CYCLES[5] = {
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
    uint8_t Ppu_State = VBLANK; // not sure what it actually starts in

    // Cycles since the last time the PPU actually ran.
    int cyclesSinceLastExec = 0;
    
    // x coord for pixel transfer
    uint16_t x = 0;

    static Color gb_colors[4];

    // PPU state machine
    void OAMScan(uint8_t *nextState);
    void PixelTransfer(uint8_t *nextState);
    void HBlank(uint8_t *nextState);
    void VBlank(uint8_t *nextState);

    void Px_RenderBackground(void);
    void Px_Window(void);
    void Px_Sprite(void);

    // Helpers
    void UpdateCycles(uint8_t state);
    bool UseUnsignedAddressing(void);

  public:
    bool Execute(uint8_t cpu_cycles_elapsed);
    int cnt = 144; // ???

    // Constructor & destructor
    Ppu(Bus* bus_, Display* disp_) {
      bus = bus_;
      disp = disp_;
    }
};

#endif
