  
/* ░█▀▀░█▀▄░█▀█░█▀█░█░█░▀█▀░█▀▀░█▀▀*/ 
/* ░█░█░█▀▄░█▀█░█▀▀░█▀█░░█░░█░░░▀▀█*/
/* ░▀▀▀░▀░▀░▀░▀░▀░░░▀░▀░▀▀▀░▀▀▀░▀▀▀*/

#include <SDL2/SDL.h>
#include "ppu.h"
#include "bus.h"
#include "display/gba-sdl.h"
#include "defines.h"

/*                  240                       68
 *          ◄───────────────────────────► ◄──────────►
 *       ▲  ┌────────────────────────────┐────────────┐
 *       │  │                            │            │
 *       │  │                            │            │
 *       │  │                            │            │
 *       │  │                            │            │
 *   160 │  │       PIXEL TRANSFER       │   HBLANK   │
 *       │  │                            │            │
 *       │  │                            │            │
 *       │  │                            │            │
 *       │  │                            │            │
 *       ▼  ├────────────────────────────┘════════════╡
 *       ▲  │                            ║            │
 *       │  │          VBLANK            ║            │
 *    68 │  │                            ║            │
 *       ▼  └────────────────────────────╨────────────┘
 */

/* Ppu::Execute
 * Draws 1 frame to the screen */
bool Ppu::Execute(uint8_t cpu_cycles_elapsed)
{
  cycles_since_last_exec += cpu_cycles_elapsed;
  //std::cout << "csle: " << (int) cycles_since_last_exec << std::endl;

  while (CanExecute()) {
    //std::cout << "State: " << PpuStates_Str[Ppu_State] << "; LY: " << (int) bus->read(LY) << std::endl;

    switch (Ppu_State) {
      case OAM_SCAN:
        OAMScan();
        break;
      case PIXEL_TRANSFER:
        PixelTransfer();
        break;
      case HBLANK:
        HBlank();
        break;
      case VBLANK:
        VBlank();
        break;
      default: break;
    }
  }

  return EXIT_SUCCESS;
}

/* OAM scan
 * Search through OAM (object access memory) for sprites to draw.
 * Up to 10 sprites can be drawn per scanline. Excess sprites are ignored.
 * Takes 40 CPU cycles. 160 bytes in OAM. */
void Ppu::OAMScan(void)
{
  Ppu_State = PIXEL_TRANSFER;

  uint16_t ly = bus->read(LY);

  // In actual hardware, a DMA transfer copies CPU OAM to 
  // PPU OAM, but here we just read directly from CPU OAM
  // because it's a software emulator.
  for (int i = OAM_START; i < OAM_START + (OAM_BYTES * 4); i += 4) {
    uint8_t ypos = bus->read(i);      // Sprite vertical pos on screen + 16
    uint8_t xpos = bus->read(i + 1);  // H pos on screen + 8
    uint8_t tile = bus->read(i + 2);  // Tile index
    uint8_t flag = bus->read(i + 3);  // Attributes and flags

    if (ly == ypos) {
      // std::cout << "oam thingy at ly " << ly << std::endl;
    }
  }

  UpdateCycles(OAM_SCAN_CYCLES);
}

/* Ppu::PixelTransfer()
 * Draw pixels to the screen.
 * Each pixel takes 4 cycles to draw. */
void Ppu::PixelTransfer(void)
{
  ++x;

  // this->SDL_Render();

  if (x >= PX_TRANSFER_X_DURATION) Ppu_State = HBLANK;

  UpdateCycles(PX_TRANSFER_CYCLES);
}

/* Ppu::HBlank */
void Ppu::HBlank(void)
{
  ++x;

  // Leave HBlank when x > 308
  if (x >= PX_TRANSFER_X_DURATION + HBLANK_X_DURATION) {
    // Reset x coordinate and increment scanline count
    x = 0;
    uint16_t ly = bus->read(LY);
    bus->write(LY, ++ly);

    if (ly >= PX_TRANSFER_Y_DURATION) {
      Ppu_State = VBLANK;
    } else {
      Ppu_State = OAM_SCAN;
    }
  }

  UpdateCycles(HBLANK_CYCLES);
}

/* Ppu::VBlank */
void Ppu::VBlank(void)
{
  // Decrement cnt
  cnt -= 2;

  // Increment LY
  uint8_t ly = bus->read(LY);
  if (cnt <= 0) {
    bus->write(LY, ++ly);
    cnt = 226;
  }

  if (ly >= PX_TRANSFER_Y_DURATION + VBLANK_Y_DURATION) {
    Ppu_State = OAM_SCAN;
  }

  UpdateCycles(VBLANK_CYCLES);
}

/* Ppu::UpdateCycles
 * Updates cycles_since_last_exec with the number of cycles taken. */
void Ppu::UpdateCycles(uint16_t cycles_taken)
{
  cycles_since_last_exec -= cycles_taken;
  if (cycles_since_last_exec < 0) cycles_since_last_exec = 0;
}

/* Ppu::CanExecute
 * Determines if the PPU can run based on the number of cycles since last execution and the number of
 * cycles that the current PPU state takes. */
bool Ppu::CanExecute(void)
{
  switch(Ppu_State) {
    case OAM_SCAN:
      return cycles_since_last_exec >= OAM_SCAN_CYCLES;
      break;
    case PIXEL_TRANSFER:
      return cycles_since_last_exec >= PX_TRANSFER_CYCLES;
      break;
    case HBLANK:
      return cycles_since_last_exec >= HBLANK_CYCLES;
      break;
    case VBLANK:
      return cycles_since_last_exec >= VBLANK_CYCLES;
      break;
    default:
      return false;
      break;
  }
}

