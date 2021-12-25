
#include <vector>

#include "ppu.h"

/* Ppu::getFlag
 * Gets LCD reg flags. */
uint8_t Ppu::getFlag(CtrlFlags flag) {
    uint8_t f = bus->read(LCD_CTRL_REG_ADDRESS);
    return (f & (1 << flag)) >> flag;
}
uint8_t Ppu::getFlag(StatFlags flag) {
    uint8_t f = bus->read(LCD_CTRL_REG_ADDRESS);
    return (f & (1 << flag)) >> flag;
}

/* Ppu::setFlag
 * Sets LCD reg flags. */
void Ppu::setFlag(CtrlFlags flag, bool val) {
    uint8_t f = bus->read(LCD_STAT_REG_ADDRESS);
    f &= ~(1 << flag);  // set bit to 0
    f |= (val << flag); // so we can set bit to val
}
void Ppu::setFlag(StatFlags flag, bool val) {
    uint8_t f = bus->read(LCD_STAT_REG_ADDRESS);
    f &= ~(1 << flag);  // set bit to 0
    f |= (val << flag); // so we can set bit to val
}

/*  Ppu::execute
 *  Emulates the number of cycles passed into the function. */
void Ppu::execute(uint8_t cyclesToRun) {
    std::cout << "\tPPU state:" << ppuState << std::endl;
    for( int i = 0; i < cyclesToRun; ++i) {
        switch( ppuState ) {
            case OAM_SCAN:  oam_scan(); break;
            case VBLANK:    vblank();   break;
            case DRAW:      draw();     break;
            case HBLANK:    hblank();   break;
            default: break;
        }
        ++currCycleCount;   
    }
}

/*  Ppu::oam_scan()
 *  Emulates one oam scan cycle.
 *  Entered at the start of every scanline (except vblank) before pixels are drawn
 *  to screen. This mode searches OAM memory for sprites that should be rendered 
 *  on the current scanline and stores them in a buffer. 
 *  Takes 80 t-cycles (40 OAM entries; new OAM entry checked every 2 t-cycles). */
void Ppu::oam_scan() {
    // Scan every 2 cycles, aka every other cycle
    // So just toggle doCycle flag each cycle.
    doCycle ^= 1;
    if (!doCycle) return;

    // Read new oam entry
    uint8_t y_pos = bus->read(oam_position);
    uint8_t x_pos = bus->read(++oam_position);
    uint8_t tile_index = bus->read(++oam_position);
    uint8_t att_flags = bus->read(++oam_position);

    // Check conditions to add to oam buffer
    uint8_t ly = bus->read(LY_ADDRESS);

    // Determine sprite height from SPRITE_SIZE stat flags
    uint8_t height_flag = getFlag(SPRITE_SIZE);
    uint8_t sprite_height = (height_flag == 0) ? 8 : 16;
    
    bool validXPos = x_pos > 0;
    bool validYPos = (ly + 16) >= y_pos;
    bool validYPos_SpriteHeight = (ly + 16) < (y_pos + sprite_height);
    bool bufferNotFull = oam_buffer.size() < 10;

    // Push to oam_buffer if all conditions true
    if (validXPos && validYPos && validYPos_SpriteHeight && bufferNotFull) {
        std::array<uint8_t, 4> sprite = { y_pos, x_pos, tile_index, att_flags };
        oam_buffer.push_back(sprite);
    }

    // Check state
    if ( oam_position >= OAM_ADDRESS_END ) {
        ppuState = DRAW;
        oam_position = OAM_ADDRESS_START;
    }
}

/*  Ppu::draw
 *  PPU transfers pixels to LCD.
 *  Duration of this mode varies depending on multiple variables. */
void Ppu::draw() {
    // Execute one step of everything every 2 cycles, aka every other cycle
    // So just toggle doCycle flag each cycle.
    doCycle ^= 1;
    if (doCycle) {
        // If this x position has a sprite, call sprite fetch
        

        // Else call bg fetch
        bg_fetch();
    }
    
    // Push to LCD
    if (bgFifoPxCount != 0) {

    }
}

/*  Ppu::bg_fetch
 *  Loads 8-pixel bg pixel FIFO with data. 
 *  Remember: this fetches a row of a TILE, which is already 8 pixels! */
void Ppu::bg_fetch() {
    switch( bgFetchState ) {
        case FETCH_TILE_NO: bg_fetchTileNo(); break;
        case FETCH_DATA_LO: bg_fetchTileDataLo(); break;
        case FETCH_DATA_HI: bg_fetchTileDataHi(); break;
        case FETCH_PUSH:    bg_fetchPush(); break;
        default: break;
    }   
}

/*  Ppu::bg_fetchTileNo
 *  Fetch and store tile number of tile number to be used */
void Ppu::bg_fetchTileNo() {
    bgFetchState = FETCH_DATA_LO;
}

/*  Ppu::bg_fetchTileDataLo
 *  Fetch first byte of tile data */
void Ppu::bg_fetchTileDataLo() {
    bgFetchState = FETCH_DATA_HI;
}

/*  Ppu::bg_fetchTileDataHi
 *  Fetch second byte of tile data
 *  The first time this step completes on a scanline, state is 
 *  reset to fetchTileNo. */
void Ppu::bg_fetchTileDataHi() {
    if ( isFirstScanlineFetch ) {
        bgFetchState = FETCH_TILE_NO;
    }
    isFirstScanlineFetch = false;
}

/*  Ppu::bg_fetchPush
 *  Only pushes to FIFO if FIFO is empty.
 *  FIFO ONLY FILLS A FULL 8 PIXELS AT A TIME. */
void Ppu::bg_fetchPush() {
    if ( bgFifoPxCount == 0 ) {
        bgFifo = bgTile;
        bgFifoPxCount = 8;
        bgFetchState = FETCH_TILE_NO;
    }
}

void Ppu::sprite_fetch() {
    
}

void Ppu::vblank() {

}

void Ppu::hblank() {

}
