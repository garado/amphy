
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
  bgp   = bus->GetAddressPointer(0xFF47);
  obp0  = bus->GetAddressPointer(0xFF48);
  obp1  = bus->GetAddressPointer(0xFF49);
  lyc   = bus->GetAddressPointer(0xFF45);
  *ly   = 0;
  spritesOnScanline.clear();
  disp->Clear(&gb_colors[0]);
}

/* @Function Ppu::Execute
 * @param cpuCyclesElapsed
 * @brief Runs PPU until it catches up to the CPU. 
 *    1 dot == 1 t-cycle. Normally when this is called,
 *    cpuCyclesElapsed == 4, so the state machine should
 *    tick 4 times. */
void Ppu::Execute(u8 cpuCyclesElapsed) {
  // Display white
  if (BIT_TEST(*lcdc, LCDC_EN) == false) {
    if (disp->cleared == false) {
      disp->Clear(&gb_colors[0]);
    }
  }

  for (u8 i = 0; i < cpuCyclesElapsed; i++) {
    u8 nextState = NO_TRANSITION;

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

    dotsSinceStateSwitch++;

    // Handle STAT LY=LYC interrupt
    if (*ly == *lyc && BIT_TEST(*stat, STAT_LYC_INTR)) {
      *intf = BIT_SET(*intf, INTF_STAT_IRQ);
    }

    // Set variables and interrupts for state transitions
    if (nextState != NO_TRANSITION) {
      // Set STAT mode flags
      // Need to decrement nextState because enum is offset
      // by 1 (0 == NoTransition)
      *stat = (*stat & 0xFC) | (nextState - 1);

      u8 setStatIntr = false;

      if (nextState == OAM_SCAN) {
        x = 0;
        (*ly)++;
        if (doIncrementWcnt) wcnt++;
        spritesOnScanline.clear();
        if (BIT_TEST(*stat, STAT_OAM_INTR)) setStatIntr = true;

        if (ppuState == VBLANK) {
          *ly = 0;
          cnt = 226;
        }
      }

      if (nextState == VBLANK) {
        *intf = BIT_SET(*intf, INTF_VBLANK_IRQ);
        if (BIT_TEST(*stat, STAT_VBLANK_INTR)) setStatIntr = true;
        disp->HandleEvent();
        disp->Render();
      }

      if (nextState == HBLANK) {
        doIncrementWcnt = false;
        if (BIT_TEST(*stat, STAT_HBLANK_INTR)) setStatIntr = true;
      }

      if (nextState == PIXEL_TRANSFER) {
        wcnt = 0;
        doDrawWindow = false;
      }

      if (setStatIntr) *intf = BIT_SET(*intf, INTF_STAT_IRQ);
      ppuState = nextState;
      dotsSinceStateSwitch = 0;
    }
  }
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
  // One sprite takes 2 dots to check
  if (dotsSinceStateSwitch % 2 != 0) return;

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
  if (dotsSinceStateSwitch == DOTS_OAM) {
    spriteIndex = 0;
    *nextState = PIXEL_TRANSFER;
  }
}

/* @Function Ppu::PixelTransfer
 * @brief Draw pixels to the screen. */
void Ppu::PixelTransfer(u8 *nextState)
{
  if (BIT_TEST(*lcdc, LCDC_EN) == false) {
    ++x;
    if (x >= PX_TRANSFER_X_DURATION) {
      *nextState = HBLANK;
    }
    return;
  }

  // Determining which layer gets drawn
  bool objEn = BIT_TEST(*lcdc, LCDC_OBJ_EN);
  bool winEn = BIT_TEST(*lcdc, LCDC_WIN_EN);
  bool bgEn  = BIT_TEST(*lcdc, LCDC_BG_EN);

  bool spriteDrawn = false;
  if (objEn && spritesOnScanline.size() != 0) {
    spriteDrawn = Px_RenderSprite();
  }

  if (!spriteDrawn || bgOverObj) {
    // In hardware it's wy == ly not wy <= ly for whatever reason
    if (*wy == *ly) doDrawWindow = true;

    if (bgEn || winEn) {
      Px_RenderBgWindow();
    } else { // draw white if nothing else gets drawn
      disp->DrawPixel(x, *ly, &gb_colors[0]);
    }
  }

  ++x;
  bgOverObj = false;
  if (dotsSinceStateSwitch == DOTS_PXTRANSFER) {
    *nextState = HBLANK;
  }
}

/* @Function Ppu::Px_RenderBgWindo
 * @brief Render a background or window pixel */
void Ppu::Px_RenderBgWindow(void) {
  // Set vars based on whether we're drawing bg or window
  u8 tmap_x, tmap_y, thisX, thisY;
  u16 tmap_base;

  bool winEn = BIT_TEST(*lcdc, LCDC_WIN_EN);
  if (winEn && (*wy <= *ly) && ((*wx+7) <= x)) {
    doIncrementWcnt = true;
    tmap_x = (x + *wx - 7);
    tmap_y = (*wy + wcnt);

    thisX = (x + 7 - *wx);
    thisY = wcnt;
    tmap_base = BIT_TEST(*lcdc, LCDC_WIN_TMAP)? 0x9C00 : 0x9800;
  } else {
    tmap_x = (x + *scx);
    tmap_y = (*ly + *scy);

    thisY = *scy + *ly;
    thisX = *scx + x;
    tmap_base = BIT_TEST(*lcdc, LCDC_BG_TMAP)? 0x9C00 : 0x9800;
  }

  u16 tmap_index = ((tmap_y/8) * 32) + (tmap_x / 8);
  u16 tmap_addr = tmap_base | tmap_index;

  // Read tile data using signed or unsigned tile index
  u8 tile_index = bus->Read(tmap_addr);

  u16 tdata_addr, tdata_base;
  u8 useUnsignedAddressing = BIT_TEST(*lcdc, LCDC_BGW_ADDR_MODE);
  if (useUnsignedAddressing) {
    tdata_base = 0x8000;
    tdata_addr = tdata_base + (tile_index * 16);
  } else {
    tdata_base = 0x9000;
    tdata_addr = tdata_base + ((int8_t) tile_index * 16);
  }

  // Get x and y coordinates within 8x8 tile
  u8 tile_x = 7 - (thisX % 8);
  u8 tile_y = thisY % 8;
  tdata_addr += (tile_y * 2);

  // Determine color
  u8 lsbit = BIT_GET(bus->Read(tdata_addr), tile_x);
  u8 msbit = BIT_GET(bus->Read(tdata_addr+1), tile_x);
  u8 id = (msbit << 1) | lsbit;

  // Apply color
  u8 paletteID = (*bgp >> (id * 2)) & 0b11;
  Color c = gb_colors[paletteID];

  // If a sprite was drawn at this coord and its bgOverObj
  // attribute was set, BG and Window colors 1-3 should be
  // drawn over it.
  if (bgOverObj && paletteID == 3) return;

  disp->DrawPixel(x, *ly, &c);
}

/* @Function Ppu::FindScanlineSprite
 * @return Index of sprite within spritesOnScanline
 * @brief Iterate through the sprites on this scanline and find
 * a suitable one to draw. */
u8 Ppu::Px_FindScanlineSprite(void) {
  for (u8 i = 0; i < spritesOnScanline.size(); i++) {
    Address oamAddr = spritesOnScanline[i];
    u8 spriteX = bus->Read(oamAddr + OAM_XPOS) - 8;
    if (spriteX <= x && spriteX+8 > x) {
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
  // Byte 2 of sprite data is tile index
  Address oamAddr = spritesOnScanline[idx];
  u8 tileIndex = bus->Read(oamAddr + OAM_TIDX);

  // One tile is 16 bytes; use current y position within the sprite
  // to find the correct 2 bytes to read from
  u16 cur_y_pos = *ly - bus->Read(oamAddr) + 16;

  // Bit 0 of tile index for 8x16 objects should be ignored
  bool renderingBottomTile = cur_y_pos >= 8;
  if (BIT_TEST(*lcdc, LCDC_OBJ_SIZE)) {
    if (renderingBottomTile) {
      tileIndex &= 0xFE;
    } else {
      tileIndex |= 0x01;
      cur_y_pos -= 8;
    }
  }

  // u16 tmap_index = ((tmap_y/8) * 32) + (tmap_x / 8);
  // u16 tmap_addr = tmap_base | tmap_index;
  Address tdataAddr = 0x8000 | (tileIndex * 16);

  u8 attr = bus->Read(oamAddr + OAM_ATTR);

  // Handle vertical flip
  if (BIT_TEST(attr, OAM_ATTR_YFLIP)) {
    u8 objSize = BIT_TEST(*lcdc, LCDC_OBJ_SIZE) ? 16 : 8;
    cur_y_pos = objSize - 1 - cur_y_pos;
  }

  tdataAddr += (cur_y_pos * 2);

  // Find correct column within tile
  u8 cur_x_pos = x - bus->Read(oamAddr+1) + 8;
  u8 bitpos = 7 - cur_x_pos;

  // Handle horizontal flip
  if (BIT_TEST(attr, OAM_ATTR_XFLIP)) {
    bitpos = 7 - bitpos;
  }

  u8 lsbit = BIT_GET(bus->Read(tdataAddr), bitpos);
  u8 msbit = BIT_GET(bus->Read(tdataAddr+1), bitpos);
  u8 id = (msbit << 1) | lsbit;

  // Apply color
  u8 palette = BIT_TEST(attr, OAM_ATTR_PALETTE) ? *obp1 : *obp0;
  u8 paletteID = (palette >> (id * 2)) & 0b11;
  Color c = gb_colors[paletteID];

  // Don't draw transparent (id == 0) pixels
  if (paletteID == 0) return false;

  bgOverObj = BIT_TEST(attr, OAM_ATTR_BGW_OVER_OBJ);

  disp->DrawPixel(x, *ly, &c);
  return true;
}

/* Ppu::HBlank */
void Ppu::HBlank(u8 *nextState) {
  if (*ly == *wy) doDrawWindow = true;
  if (dotsSinceStateSwitch == DOTS_HBLANK) {
    if (*ly == LY_AT_VBLANK_START) {
      *nextState = VBLANK;
    } else {
      *nextState = OAM_SCAN;
    }
  }
}

/* Ppu::VBlank */
void Ppu::VBlank(u8 *nextState) {
  if (dotsSinceStateSwitch % DOTS_VBLANK_SCANLINE == 0) {
    (*ly)++;
  }

  if (dotsSinceStateSwitch == DOTS_VBLANK_TOTAL) {
    *nextState = OAM_SCAN;
  }
}
