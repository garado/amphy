
/* █▀▀ █▀█ ▄▀█ █▀█ █░█ █ █▀▀ █▀ */
/* █▄█ █▀▄ █▀█ █▀▀ █▀█ █ █▄▄ ▄█ */

#include "ppu.h"
#include "../bus.h"
#include "../platform/platform.h"
#include "../defines.h"

extern bool testflag;

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
 * @brief Runs PPU until it catches up to the CPU */
bool Ppu::Execute(uint8_t cpu_cycles_elapsed)
{
  cycles_since_last_exec += cpu_cycles_elapsed;

  while (CanExecute()) {
    // std::cout << "State: " << PpuStates_Str[Ppu_State] << "; LY: " << (int) bus->read(LY) << std::endl;

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

/* @Function Ppu::OAMScan
 * @brief Search through OAM (object access memory) for sprites to draw.
 *    Up to 10 sprites can be drawn per scanline. Excess sprites are ignored.
 *    Takes 40 CPU cycles. 160 bytes in OAM. */
void Ppu::OAMScan(void)
{
  Ppu_State = PIXEL_TRANSFER;

  // uint16_t ly = bus->read(LY);

  // // In actual hardware, a DMA transfer copies CPU OAM to
  // // PPU OAM, but here we just read directly from CPU OAM
  // for (int i = OAM_START; i < OAM_START + (OAM_BYTES * 4); i += 4) {
  //   uint8_t ypos = bus->read(i);      // Sprite vert pos on screen + 16
  //   uint8_t xpos = bus->read(i + 1);  // H pos on screen + 8
  //   uint8_t tile = bus->read(i + 2);  // Tile index
  //   uint8_t flag = bus->read(i + 3);  // Attributes and flags

  //   if (ly == ypos) {
  //     // std::cout << "oam thingy at ly " << ly << std::endl;
  //   }

  //   bool spriteVisible = xpos != 0;
  //   bool scanlineVisible = (ly + 16 >= ypos) ^ (ly + 16 < ypos + 8);

  //   bool drawSprite = spriteVisible ^ scanlineVisible;
  // }

  UpdateCycles(OAM_SCAN_CYCLES);
}

/* Ppu::PixelTransfer()
 * Draw pixels to the screen.
 * Each pixel takes 4 cycles to draw. */
void Ppu::PixelTransfer(void)
{
  ++x;
  uint8_t ly = bus->read(LY);

  // Determine which tilemap
  uint8_t tilemap_flag = bus->BitTest(LCDC, LCDC_BIT_BG_ADDR);
  uint16_t tilemap_base = tilemap_flag ? 0x9C00 : 0x9800;

  // Determine tilemap index (coords are in pixels)
  uint8_t tilemap_y = ((ly + bus->read(SCY)) / 8) & 0xFF;
  uint8_t tilemap_x = ((bus->read(SCX) + x) / 8) & 0x1F;

  uint16_t tilemap_offset = (tilemap_y * 32) + tilemap_x;
  uint16_t tilemap_addr = tilemap_base + tilemap_offset;

  if (testflag && (bus->read(SCY) >= 0x77)) {
    printf("ly: %d\n", bus->read(LY));
    printf("%x %d %d %x %x\n", tilemap_base, tilemap_x, tilemap_y, tilemap_offset, tilemap_addr);
  }

  // Read tile data using tile index
  uint16_t tiledata_address, tiledata_base;
  if (UseUnsignedAddressing()) {
    tiledata_base = 0x8000;
    uint8_t tiledata_offset = bus->read(tilemap_addr);
    tiledata_address = tiledata_base + (tiledata_offset * 2);
  } else {
    tiledata_base = 0x9000;
    int8_t tiledata_offset = bus->read(tilemap_addr);
    tiledata_address = tiledata_base + (tiledata_offset * 2);
  }
  
  if (testflag) {
    printf("    tiledata address: %x\n", tiledata_address);
  }

  if (tiledata_address == 0x9800) {
    while(1) {}
  }

  // Now fetch stuff from tile data
  uint8_t bitpos = (bus->read(SCX) + x) % 8;
  uint8_t lsbit = (bus->read(tiledata_address) >> bitpos) & 0x1;
  uint8_t msbit = (bus->read(tiledata_address + 1) >> bitpos) & 0x1;
  uint8_t id = (msbit << 1) | msbit;

  Color color = gb_colors[id];

  disp->DrawPixel(x, bus->read(LY), &color);

  if (x >= PX_TRANSFER_X_DURATION) {
    Ppu_State = HBLANK;
    disp->Render();
  }

  UpdateCycles(PX_TRANSFER_CYCLES);
}

/* Ppu::HBlank */
void Ppu::HBlank(void)
{
  ++x;
  uint8_t ly = bus->read(LY);

  // Leave HBlank when x > 308
  if (x >= PX_TRANSFER_X_DURATION + HBLANK_X_DURATION) {
    // Reset x coordinate and increment scanline count
    x = 0;

    if (ly >= PX_TRANSFER_Y_DURATION) {
      Ppu_State = VBLANK;
    } else {
      Ppu_State = OAM_SCAN;
    }

    bus->write(LY, ++ly);
  }

  UpdateCycles(HBLANK_CYCLES);
}

/* Ppu::VBlank */
void Ppu::VBlank(void)
{
  uint8_t ly = bus->read(LY);
  // Decrement cnt
  cnt -= 2;

  // Increment LY
  if (cnt <= 0) {
    bus->write(LY, ++ly);
    cnt = 226; // TODO #define
  }

  // Set STAT interrupts if necessary
  uint8_t stat_ints_enabled = bus->read(INTF) & INT_STAT_BIT;
  if (bus->read(LY) == bus->read(LYC) && stat_ints_enabled) {
    uint8_t stat = bus->read(STAT);

    if ((stat & STAT_BIT_LYC)) {
      uint8_t newstat = stat |= STAT_BIT_LYC_FLAG;
      uint8_t newintf = bus->read(INTF) | INT_STAT_BIT;
      bus->write(STAT, newstat); 
      bus->write(INTF, newintf); 
    }
  }

  // State change to OAM_SCAN
  if (ly >= PX_TRANSFER_Y_DURATION + VBLANK_Y_DURATION) {
    Ppu_State = OAM_SCAN;
    bus->write(LY, 0);
  }

  UpdateCycles(VBLANK_CYCLES);
}

/* @Function Ppu::UpdateCycles
 * @brief Updates cycles_since_last_exec with the number of cycles taken. */
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

bool Ppu::UseUnsignedAddressing(void)
{
  return bus->BitTest(LCDC, LCDC_BIT_BGW_TD) != 0;
}
