
/* █▀▀ █▀█ ▄▀█ █▀█ █░█ █ █▀▀ █▀ */
/* █▄█ █▀▄ █▀█ █▀▀ █▀█ █ █▄▄ ▄█ */

#include "common.h"
#include "ppu.h"
#include "bus.h"
#include "platform/platform.h"
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
 *       ▼  ├────────────────────────────┘────────────┐
 *       ▲  │                                         │
 *       │  │                 VBLANK                  │
 *    68 │  │                                         │
 *       ▼  └─────────────────────────────────────────┘
 */

Color color_0 = {15, 65, 15};
Color color_1 = {48, 94, 48};
Color color_2 = {139, 172, 15};
Color color_3 = {155, 188, 15};

Color Ppu::gb_colors[4] = { color_0, color_1, color_2, color_3 };

void Ppu::Init() {
  ly    = bus->GetAddressPointer(LY);
  wx    = bus->GetAddressPointer(WX);
  wy    = bus->GetAddressPointer(WY);
  scx   = bus->GetAddressPointer(SCX);
  scy   = bus->GetAddressPointer(SCY);
  stat  = bus->GetAddressPointer(STAT);
  lcdc  = bus->GetAddressPointer(LCDC);
  intf  = bus->GetAddressPointer(INTF);
}

/* @Function Ppu::Execute
 * @param cpuCyclesElapsed
 * @brief Runs PPU until it catches up to the CPU. */
bool Ppu::Execute(uint8_t cpuCyclesElapsed) {
  cyclesSinceLastExec += cpuCyclesElapsed;
  uint8_t nextState = NO_TRANSITION;

  while (cyclesSinceLastExec >= PPU_STATE_CYCLES[ppuState]) {
    switch (ppuState) {
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

    UpdateCycles(ppuState);
    ppuCyclesElapsed += PPU_STATE_CYCLES[ppuState];
  }

  // Set mode flags and interrupts for state transitions
  if (nextState != NO_TRANSITION) {
    ppuState = nextState;

    // Mode flags
    // decrement nextState because it's offset by 1
    *stat = (*stat & 0b00) | (nextState - 1);

    // VBlank interrupt
    *intf = BIT_SET(*intf, INTF_VBLANK_IRQ);

    // STAT interrupts
    // TODO #defines cause wtf are these numbers
    uint8_t setStatIntr = false;
    if ((nextState == HBLANK) && (*stat & 0b0001000)) {
      setStatIntr = true;
    } else if ((nextState == VBLANK) && (*stat & 0b0010000)) {
      setStatIntr = true;
    } else if ((nextState == OAM_SCAN) && (*stat & 0b0100000)) {
      setStatIntr = true;
    }

    if (setStatIntr) *intf = BIT_SET(*intf, INTF_STAT_IRQ);
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

void Ppu::PixelTransfer(uint8_t *nextState)
{
  if (BIT_TEST(*lcdc, LCDC_EN) == false) return;

  // Determine which layer gets drawn (sprites not implemented yet)
  bool winEn = BIT_TEST(*lcdc, LCDC_WIN_EN);
  bool bgEn = BIT_TEST(*lcdc, LCDC_BG_EN);

  if (winEn && (*wy <= *ly) && ((*wx)+7 <= x)) {
    Px_RenderWindow();
  } else if (bgEn) {
    Px_RenderBackground();
  }

  ++x;
  if (x >= PX_TRANSFER_X_DURATION) {
    *nextState = HBLANK;
  }
}

/* @Function Ppu::Px_RenderBackground
 * @brief Render a background pixel */
void Ppu::Px_RenderBackground(void)
{
  // Determine which of the 2 tilemaps to use
  // TODO #define
  uint16_t tmap_base = BIT_TEST(*lcdc, LCDC_BG_TMAP)? 0x9C00 : 0x9800;


  // Determine tilemap index 0-1024
  uint16_t tmap_y = (*ly + *scy) / 8;
  uint16_t tmap_x = (*scx + x) / 8;
  uint16_t tmap_offset = (tmap_y * 32) + tmap_x;
  uint16_t tmap_addr = tmap_base + tmap_offset;

  // printf("    x, ly: %d %d\n", x, bus->Read(LY));
  // printf("    scx, scy: %d %d\n", bus->Read(SCX), bus->Read(SCY));
  // printf("    Tilemap x, y: %d %d\n", tilemap_x, tilemap_y);
  // printf("    Tilemap addr: 0x%x\n", tilemap_addr);

  // Read tile data using tile index
  uint16_t tdata_addr, tdata_base;
  if (UseUnsignedAddressing()) {
    tdata_base = 0x8000;
    uint8_t tiledata_offset = bus->Read(tmap_addr);
    tdata_addr = tdata_base + (tiledata_offset * 16);
  } else {
    tdata_base = 0x9000;
    int8_t tiledata_offset = bus->Read(tmap_addr);
    tdata_addr = tdata_base + (tiledata_offset * 16);
  }

  // printf("    Tiledata base: 0x%x\n", tdata_base);
  // printf("    Tiledata addr: 0x%x\n", tdata_addr);

  uint8_t bitpos = 7 - ((*scx + x) % 8); // doesn't work without the 7?
  uint8_t tile_y = ((*scy + *ly) % 8);
  tdata_addr += (tile_y * 2);
  // printf("    Tile x y: %d %d\n", bitpos, tile_y);

  uint8_t lsbit = BIT_GET(bus->Read(tdata_addr), bitpos);
  uint8_t msbit = BIT_GET(bus->Read(tdata_addr+1), bitpos);
  uint8_t id = (msbit << 1) | msbit;

  disp->DrawPixel(x, *ly, &gb_colors[id]);
}

void Ppu::Px_RenderWindow(void) {
  // Determine which of the 2 tilemaps to use
  uint16_t tmap_base = BIT_TEST(*lcdc, LCDC_WIN_TMAP)? 0x9C00 : 0x9800;

  // Determine tilemap index 0-1024
  uint16_t tmap_y = (*ly + *wy) / 8;
  uint16_t tmap_x = (*wx + x) / 8;
  uint16_t tmap_offset = (tmap_y * 32) + tmap_x;
  uint16_t tmap_addr = tmap_base + tmap_offset;

  // Read tile data using tile index
  uint16_t tdata_addr, tdata_base;
  if (UseUnsignedAddressing()) {
    tdata_base = 0x8000;
    uint8_t tiledata_offset = bus->Read(tmap_addr);
    tdata_addr = tdata_base + (tiledata_offset * 16);
  } else {
    tdata_base = 0x9000;
    int8_t tiledata_offset = bus->Read(tmap_addr);
    tdata_addr = tdata_base + (tiledata_offset * 16);
  }

  uint8_t bitpos = 7 - ((*wx + x) % 8); // doesn't work without the 7?
  uint8_t tile_y = ((*wy + *ly) % 8);
  tdata_addr += (tile_y * 2);

  uint8_t lsbit = BIT_GET(bus->Read(tdata_addr), bitpos);
  uint8_t msbit = BIT_GET(bus->Read(tdata_addr+1), bitpos);
  uint8_t id = (msbit << 1) | msbit;

  disp->DrawPixel(x, *ly, &gb_colors[id]);
}

void Ppu::Px_RenderSprite(void)
{

}

/* Ppu::HBlank */
void Ppu::HBlank(uint8_t *nextState) {
  ++x;

  // Leave HBlank when x > 308
  if (x >= PX_TRANSFER_X_DURATION + HBLANK_X_DURATION) {
    // Reset x coordinate and increment scanline count
    x = 0;
    (*ly)++;

    if (*ly >= PX_TRANSFER_Y_DURATION) {
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
  if (cnt <= 0) {
    (*ly)++;
    cnt = 226; // TODO #define
  }

  // State change to OAM_SCAN
  if (*ly >= PX_TRANSFER_Y_DURATION + VBLANK_Y_DURATION) {
    *nextState = OAM_SCAN;
    bus->Write(LY, 0);
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
  return bus->BitTest(LCDC, LCDC_BGW_ADDR_MODE);
}
