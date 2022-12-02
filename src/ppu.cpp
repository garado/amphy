
/* ░█▀▀░█▀▄░█▀█░█▀█░█░█░▀█▀░█▀▀░█▀▀*/ 
/* ░█░█░█▀▄░█▀█░█▀▀░█▀█░░█░░█░░░▀▀█*/
/* ░▀▀▀░▀░▀░▀░▀░▀░░░▀░▀░▀▀▀░▀▀▀░▀▀▀*/

#include "ppu.h"

#define PPU_DEBUG

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

  while (this->CanExecute()) {
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
 * Takes 40 CPU cycles. */
void Ppu::OAMScan(void)
{
  Ppu_State = PIXEL_TRANSFER;

  UpdateCycles(OAM_SCAN_CYCLES);
}

/* PIXEL TRANSFER 
 * Draw pixels to the screen.
 * Each pixel takes 4 cycles to draw. */
void Ppu::PixelTransfer(void)
{
  ++x;

  if (x >= PX_TRANSFER_X_DURATION) Ppu_State = HBLANK;

  UpdateCycles(PX_TRANSFER_CYCLES);
}

/* Ppu::HBlank */
void Ppu::HBlank(void)
{
  ++x;

  //std::cout << "x: " << (int) x << std::endl;

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
  // Increment LY
  uint8_t ly = bus->read(LY);
  bus->write(LY, ++ly);

  if (ly >= PX_TRANSFER_Y_DURATION + VBLANK_Y_DURATION) {
    Ppu_State = OAM_SCAN;
  }

  UpdateCycles(VBLANK_CYCLES);
}

/* Ppu::UpdateCycles
 * Updates cycles_since_last_exec with the number of cycles taken. */
void Ppu::UpdateCycles(uint8_t cycles_taken)
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
      return 0;
      break;
  }
}
