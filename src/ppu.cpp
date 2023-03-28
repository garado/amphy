
/* █▀▀ █▀█ ▄▀█ █▀█ █░█ █ █▀▀ █▀ */
/* █▄█ █▀▄ █▀█ █▀▀ █▀█ █ █▄▄ ▄█ */

#include "ppu.h"
#include "bus.h"
#include "platform/platform.h"
#include "defines.h"
#include <cstdio>

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
 *       ▲  │                                         │
 *       │  │          VBLANK                         │
 *    68 │  │                                         │
 *       ▼  └────────────────────────────╨────────────┘
 */

Color color_0 = {15, 65, 15};
Color color_1 = {48, 94, 48};
Color color_2 = {139, 172, 15};
Color color_3 = {155, 188, 15};

Color Ppu::gb_colors[4] = { color_0, color_1, color_2, color_3 };

/* @Function Ppu::Execute
 * @param cpu_cycles_elapsed
 * @brief Runs PPU until it catches up to the CPU. */
bool Ppu::Execute(uint8_t cpu_cycles_elapsed)
{
  cyclesSinceLastExec += cpu_cycles_elapsed;
  uint8_t nextState = NO_TRANSITION;

  while (cyclesSinceLastExec >= PPU_STATE_CYCLES[Ppu_State]) {
    switch (Ppu_State) {
      case OAM_SCAN:
        OAMScan(&nextState);
        break;
      case PIXEL_TRANSFER:
        PixelTransfer(&nextState);
        break;
      case HBLANK:
        HBlank(&nextState);
        break;
      case VBLANK:
        VBlank(&nextState);
        break;
      default: break;
    }

    UpdateCycles(Ppu_State);
  }

  // Set mode flags and interrupts for state transitions
  if (nextState != NO_TRANSITION) {
    Ppu_State = nextState;
    uint8_t stat = bus->read(STAT);

    // Mode flags
    // Decrement nextState because it's offset by 1
    stat = (stat & 0b00) | (nextState-1);
    bus->write(STAT, stat);

    // VBlank interrupt
    if (bus->BitTest(INTE, INTE_VBLANK_IE)) {
      bus->BitSet(INTF, INTF_VBLANK_IRQ);
    }

    // STAT interrupts
    // TODO #defines
    uint8_t setStatIntr = false;
    if ((nextState == HBLANK) && (stat & 0b0001000)) {
      setStatIntr = true;
    } else if ((nextState == VBLANK) && (stat & 0b0010000)) {
      setStatIntr = true;
    } else if ((nextState == OAM_SCAN) && (stat & 0b0100000)) {
      setStatIntr = true;
    }

    uint8_t statIntrEnabled = bus->BitTest(INTE, INTE_STAT_IE);
    if (setStatIntr && statIntrEnabled) {
      bus->BitSet(INTF, INTF_STAT_IRQ);
    }
  }

  return EXIT_SUCCESS;
}

/* @Function Ppu::UpdateCycles
 * @brief Updates cyclesSinceLastExec with the number of cycles taken. */
void Ppu::UpdateCycles(uint8_t state)
{
  uint16_t cycles_taken = PPU_STATE_CYCLES[state];
  cyclesSinceLastExec -= cycles_taken;
  if (cyclesSinceLastExec < 0) cyclesSinceLastExec = 0;
}

/* █▀ ▀█▀ ▄▀█ ▀█▀ █▀▀    █▀▄▀█ ▄▀█ █▀▀ █░█ █ █▄░█ █▀▀ */
/* ▄█ ░█░ █▀█ ░█░ ██▄    █░▀░█ █▀█ █▄▄ █▀█ █ █░▀█ ██▄ */

/* @Function Ppu::OAMScan
 * @brief Search through OAM (object access memory) for sprites to draw.
 *    Up to 10 sprites can be drawn per scanline. Excess sprites are ignored.
 *    Takes 40 CPU cycles. 160 bytes in OAM. */
void Ppu::OAMScan(uint8_t *nextState)
{
  *nextState = PIXEL_TRANSFER;
}

/* Ppu::PixelTransfer()
 * Draw pixels to the screen. This function handles
 * drawing all layers. Each pixel takes 4 cycles to draw. */
void Ppu::PixelTransfer(uint8_t *nextState)
{
  Px_RenderBackground();

  // Check if BG enabled
  // if (!bus->BitTest(LCDC, LCDC_BG_EN)) {
  //   disp->DrawPixel(x, bus->read(LY), &color_0);
  //   ++x;
  //   if (x >= PX_TRANSFER_X_DURATION) {
  //     *nextState = HBLANK;
  //   }
  //   return;
  // }

  ++x;
  if (x >= PX_TRANSFER_X_DURATION) {
    *nextState = HBLANK;
  }
}

/* @Function Ppu::Px_RenderBackground
 * @brief Render a background pixel */
void Ppu::Px_RenderBackground(void)
{
  uint8_t ly = bus->read(LY);

  // Determine which of the 2 tilemaps to use
  // TODO #define
  uint8_t tilemap_flag = bus->BitTest(LCDC, LCDC_BG_TMAP);
  uint16_t tilemap_base = tilemap_flag ? 0x9C00 : 0x9800;

  // printf("Tilemap base addr: 0x%x\n", tilemap_base);

  // Determine tilemap index (coords are in bytes)
  uint16_t tilemap_y = (ly + bus->read(SCY)) / 8;
  uint16_t tilemap_x = (bus->read(SCX) + x) / 8;
  uint16_t tilemap_offset = (tilemap_y * 32) + tilemap_x;
  uint16_t tilemap_addr = tilemap_base + tilemap_offset;

  // printf("x, ly: %d %d\n", x, bus->read(LY));
  // printf("    scx, scy: %d %d\n", bus->read(SCX), bus->read(SCY));
  // printf("    Tilemap x, y: %d %d\n", tilemap_x, tilemap_y);
  // printf("    Tilemap addr: 0x%x\n", tilemap_addr);

  // Read tile data using tile index
  uint16_t tiledata_address, tiledata_base;
  if (UseUnsignedAddressing()) {
    tiledata_base = 0x8000;
    uint8_t tiledata_offset = bus->read(tilemap_addr);
    tiledata_address = tiledata_base + (tiledata_offset * 16);
  } else {
    tiledata_base = 0x9000;
    int8_t tiledata_offset = bus->read(tilemap_addr);
    tiledata_address = tiledata_base + (tiledata_offset * 16);
  }

  // printf("    Tiledata base: 0x%x\n", tiledata_base);
  // printf("    Tiledata addr: 0x%x\n", tiledata_address);

  uint8_t bitpos = 7 - ((bus->read(SCX) + x) % 8);
  uint8_t tile_y = ((bus->read(SCY) + ly) % 8);
  tiledata_address += (tile_y * 2);
  // printf("    Tile x y: %d %d\n", bitpos, tile_y);

  uint8_t lsbit = (bus->read(tiledata_address) >> bitpos) & 0x1;
  uint8_t msbit = (bus->read(tiledata_address + 1) >> bitpos) & 0x1;
  uint8_t id = (msbit << 1) | msbit;

  Color color = gb_colors[id];

  //disp->Render()
  disp->DrawPixel(x, bus->read(LY), &color);
}

void Ppu::Px_Window(void)
{

}

void Ppu::Px_Sprite(void)
{

}

/* Ppu::HBlank */
void Ppu::HBlank(uint8_t *nextState)
{
  ++x;
  uint8_t ly = bus->read(LY);

  // Leave HBlank when x > 308
  if (x >= PX_TRANSFER_X_DURATION + HBLANK_X_DURATION) {
    // Reset x coordinate and increment scanline count
    x = 0;
    bus->write(LY, ++ly);

    if (ly >= PX_TRANSFER_Y_DURATION) {
      *nextState = VBLANK;
      disp->Render();
    } else {
      *nextState = OAM_SCAN;
    }
  }
}

/* Ppu::VBlank */
void Ppu::VBlank(uint8_t *nextState)
{
  // Decrement cnt
  cnt -= 2; // TODO #define

  // Increment LY
  uint8_t ly = bus->read(LY);
  if (cnt <= 0) {
    bus->write(LY, ++ly);
    cnt = 226; // TODO #define
  }

  // State change to OAM_SCAN
  if (ly >= PX_TRANSFER_Y_DURATION + VBLANK_Y_DURATION) {
    *nextState = OAM_SCAN;
    bus->write(LY, 0);
    cnt = 226;
  }
}

/* █░█ █▀▀ █░░ █▀█ █▀▀ █▀█ █▀ */
/* █▀█ ██▄ █▄▄ █▀▀ ██▄ █▀▄ ▄█ */

/* @Function Ppu::UseUnsignedAddressing
 * @brief Return true if should use unsigned addressing (base 
 *    address $8000) or false for signed addressing (base
 *    address $$9000) */
bool Ppu::UseUnsignedAddressing(void)
{
  return bus->BitTest(LCDC, LCDC_BGW_ADDR_MODE) == 0;
}
