
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

Color color_3 = {0x08, 0x18, 0x20};
Color color_2 = {0x34, 0x68, 0x56};
Color color_1 = {0x88, 0xC0, 0x70};
Color color_0 = {0xE0, 0xF8, 0xD0};

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

  spritesOnScanline.clear();
}

/* @Function Ppu::Execute
 * @param cpuCyclesElapsed
 * @brief Runs PPU until it catches up to the CPU. */
bool Ppu::Execute(u8 cpuCyclesElapsed) {
  cyclesSinceLastExec += cpuCyclesElapsed;
  u8 nextState = NO_TRANSITION;

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

    if (nextState == OAM_SCAN) {
      spritesOnScanline.clear();
    }

    // Mode flags
    // decrement nextState because it's offset by 1
    *stat = (*stat & 0b00) | (nextState - 1);

    // VBlank interrupt
    *intf = BIT_SET(*intf, INTF_VBLANK_IRQ);

    // STAT interrupts
    // TODO #defines cause wtf are these numbers
    u8 setStatIntr = false;
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
void Ppu::UpdateCycles(u8 state)
{
  u16 cycles_taken = PPU_STATE_CYCLES[state];
  cyclesSinceLastExec -= cycles_taken;
  if (cyclesSinceLastExec < 0) cyclesSinceLastExec = 0;
}

/* █▀ ▀█▀ ▄▀█ ▀█▀ █▀▀    █▀▄▀█ ▄▀█ █▀▀ █░█ █ █▄░█ █▀▀ */
/* ▄█ ░█░ █▀█ ░█░ ██▄    █░▀░█ █▀█ █▄▄ █▀█ █ █░▀█ ██▄ */

/* @Function Ppu::OAMScan
 * @brief Search through OAM (object access memory) for sprites to draw.
 *    Up to 10 sprites can be drawn per scanline. Excess sprites are ignored.
 *    40 sprites/160 bytes in OAM. Function is called 40 times. */
void Ppu::OAMScan(u8 *nextState) {
  // Keep track of which of the 40 sprites we are checking
  static u8 spriteIndex = 0;

  // Size of sprite is 4 bytes, byte 0 is y pos+16
  Address addr = OAM_START + (spriteIndex * 4);
  u8 ypos = bus->Read(addr) - 16;

  // Check if sprite is on current scanline
  u8 spriteHeight = BIT_TEST(*lcdc, LCDC_OBJ_SIZE) ? 16 : 8;
  bool onScanline = ypos <= *ly && ypos+spriteHeight >= *ly;
  if (onScanline && spritesOnScanline.size() < 10) {
    spritesOnScanline.push_back(addr);
  }

  ++spriteIndex;
  if (spriteIndex == 40) {
    spriteIndex = 0;
    *nextState = PIXEL_TRANSFER;
  }
}

/* @Function Ppu::PixelTransfer
 * @brief Draw pixels to the screen. */
void Ppu::PixelTransfer(u8 *nextState)
{
  if (BIT_TEST(*lcdc, LCDC_EN) == false) return;

  // Determining which layer gets drawn

  bool objEn = BIT_TEST(*lcdc, LCDC_OBJ_EN);
  bool winEn = BIT_TEST(*lcdc, LCDC_WIN_EN);
  bool bgEn  = BIT_TEST(*lcdc, LCDC_BG_EN);

  bool spriteDrawn = false;
  if (objEn && spritesOnScanline.size() != 0) {
    spriteDrawn = Px_RenderSprite();
  }

  if (!spriteDrawn) {
    if (winEn && (*wy <= *ly) && ((*wx)+7 <= x)) {
      Px_RenderWindow();
    } else if (bgEn) {
      Px_RenderBackground();
    }
  }

  ++x;
  if (x >= PX_TRANSFER_X_DURATION) {
    *nextState = HBLANK;
  }
}

/* @Function Ppu::Px_RenderBackground
 * @brief Render a background pixel */
void Ppu::Px_RenderBackground(void) {
  // Determine which of the 2 tilemaps to use
  u16 tmap_base = BIT_TEST(*lcdc, LCDC_BG_TMAP)? 0x9C00 : 0x9800;

  // Determine address within 32-bit tilemap
  u8 tmap_y = (*ly + *scy) / 8;
  u8 tmap_x = (x + *scx) / 8;
  u16 tmap_offset = (tmap_y * 32) + tmap_x;
  u16 tmap_addr = tmap_base + tmap_offset;

  // Read tile data using signed or unsigned tile index
  u16 tdata_addr, tdata_base;
  if (UseUnsignedAddressing()) {
    tdata_base = 0x8000;
    u8 tiledata_offset = bus->Read(tmap_addr);
    tdata_addr = tdata_base + (tiledata_offset * 16);
  } else {
    tdata_base = 0x9000;
    int8_t tiledata_offset = bus->Read(tmap_addr);
    tdata_addr = tdata_base + ((int8_t) tiledata_offset * (int16_t) 16);
  }

  // Get x and y coordinates within 8x8 tile
  u8 tile_x = 7 - (x % 8);
  u8 tile_y = *ly % 8;
  tdata_addr += (tile_y * 2);

  // Determine color
  u8 lsbit = BIT_GET(bus->Read(tdata_addr), tile_x);
  u8 msbit = BIT_GET(bus->Read(tdata_addr+1), tile_x);
  u8 id = (msbit << 1) | lsbit;

  disp->DrawPixel(x, *ly, &gb_colors[id]);
}

void Ppu::Px_RenderWindow(void) {
  // Determine which of the 2 tilemaps to use
  u16 tmap_base = BIT_TEST(*lcdc, LCDC_WIN_TMAP)? 0x9C00 : 0x9800;

  // Determine tilemap index 0-1024
  u16 tmap_y = (*ly + *wy) / 8;
  u16 tmap_x = (*wx + x) / 8;
  u16 tmap_offset = (tmap_y * 32) + tmap_x;
  u16 tmap_addr = tmap_base + tmap_offset;

  // Read tile data using tile index
  u16 tdata_addr, tdata_base;
  if (UseUnsignedAddressing()) {
    tdata_base = 0x8000;
    u8 tiledata_offset = bus->Read(tmap_addr);
    tdata_addr = tdata_base + (tiledata_offset * 16);
  } else {
    tdata_base = 0x9000;
    int8_t tiledata_offset = bus->Read(tmap_addr);
    tdata_addr = tdata_base + (tiledata_offset * 16);
  }

  u8 bitpos = 7 - ((*wx + x) % 8); // doesn't work without the 7?
  u8 tile_y = ((*wy + *ly) % 8);
  tdata_addr += (tile_y * 2);

  u8 lsbit = BIT_GET(bus->Read(tdata_addr), bitpos);
  u8 msbit = BIT_GET(bus->Read(tdata_addr+1), bitpos);
  u8 id = (msbit << 1) | msbit;

  disp->DrawPixel(x, *ly, &gb_colors[id]);
}

/* @Function Ppu::FindScanlineSprite
 * @return Index of sprite within spritesOnScanline
 * @brief Iterate through the sprites on this scanline and find
 * a suitable one to draw. */
u8 Ppu::Px_FindScanlineSprite(void) {
  for (u8 i = 0; i < spritesOnScanline.size(); i++) {
    Address spriteAddr = spritesOnScanline[i];
    u8 spriteX = bus->Read(spriteAddr + 1) - 8;
    if (spriteX < x && spriteX+8 > x) {
      return i; 
    }
  }

  return 0xFF;
}

/* @Function Ppu::RenderSprite */
bool Ppu::Px_RenderSprite(void) {
  // All sprites on scanline are stored in spritesOnScanline vec
  // Figure out which of them to draw
  u8 idx = Px_FindScanlineSprite();
  if (idx == 0xFF) return false;

  // Now find the tile data address
  // Byte 2 of sprite data is tile data offset
  Address oamAddr = spritesOnScanline[idx];
  Address tdataAddr = 0x8000 | (bus->Read(oamAddr+2)*16);

  // One tile is 16 bytes; use current y position within the sprite
  // to find the correct 2 bytes to read from
  u16 cur_y_pos = *ly - bus->Read(oamAddr) + 16;
  tdataAddr += (cur_y_pos * 2);

  // Find correct column within tile
  u8 cur_x_pos = x - bus->Read(oamAddr+1) + 8;
  u8 bitpos = 7 - cur_x_pos;

  u8 lsbit = BIT_GET(bus->Read(tdataAddr), bitpos);
  u8 msbit = BIT_GET(bus->Read(tdataAddr+1), bitpos);
  u8 id = (msbit << 1) | msbit;

  disp->DrawPixel(x, *ly, &gb_colors[id]);

  return true;
}

/* Ppu::HBlank */
void Ppu::HBlank(u8 *nextState) {
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
void Ppu::VBlank(u8 *nextState)
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
