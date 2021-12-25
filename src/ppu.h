
#include <stdio.h>
#include <iostream>
#include <array>
#include <fstream>
#include <vector>

#include "bus.h"

#ifndef PPU_H
#define PPU_H

#define OAM_ADDRESS_START 0xFE00
#define OAM_ADDRESS_END 0xFE9F
#define LCD_CTRL_REG_ADDRESS 0xFF40
#define LCD_STAT_REG_ADDRESS 0xFF41
#define SCY_ADDRESS 0xFF42
#define SCX_ADDRESS 0xFF43
#define LY_ADDRESS 0xFF44
#define LYC_ADDRESS 0xFF45
#define WY_ADDRESS 0xFF4A
#define WX_ADDRESS 0xFF4B

class Ppu {
    private:
        typedef enum PpuState { 
            VBLANK, OAM_SCAN, DRAW, HBLANK
        } PpuState;
        PpuState ppuState = OAM_SCAN;

        typedef enum FetchState {
            FETCH_TILE_NO,
            FETCH_DATA_LO,
            FETCH_DATA_HI,
            FETCH_PUSH
        } FetchState;
        FetchState bgFetchState = FETCH_TILE_NO;
        FetchState spriteFetchState = FETCH_TILE_NO;

        // LCD control flags
        typedef enum CtrlFlags {
            BG_WIN_ENABLE, SPRITE_ENABLE, SPRITE_SIZE, BG_TILEMAP_SEL,
            TILE_DATA_SEL, WIN_DISP_ENABLE, WIN_TILEMAP_SEL, LCD_ENABLE
        } CtrlFlags;
        // 0x86 0x87
        // 1000 0110
        // 1000 0111
        // 1000 0101
        // LCD status flags
        typedef enum StatFlags {
            PPU_MODE_LSB, PPU_MODE_MSB, COINCIDENCE, MODE_0_STAT_INT_ENABLE,
            MODE_1_STAT_INT_ENABLE, MODE_2_STAT_INT_ENABLE, LYC_LY_STAT_INT_ENABLE 
        } StatFlags;

        int cycleCount = 0;

        // These FIFOs store 8 pixels each.
        // 1 pixel = 2 bits, so 16 bits = 8 pixels.
        int bgFifoPxCount = 0; 
        int bgFifo_xPos = 0;    // x position of current pixel to be drawn
        uint16_t bgTile;
        uint16_t bgFifo;
        int spriteFifoPxCount = 0;
        uint16_t spriteFifo;
        uint16_t spriteTile;

        // OAM scan
        bool doCycle = true;
        std::vector<std::array<uint8_t, 4>> oam_buffer;
        uint16_t oam_position = OAM_ADDRESS_START;

        // Draw
        bool isFirstScanlineFetch = true;

        // Hblank/Vblank
        int hblankCycleCount;
        int vblankCycleCount;

    public:
        void execute(uint8_t cyclesToRun);
        Ppu(Bus* bus_) { bus = bus_; }
        ~Ppu();

    private:
        uint8_t getFlag(CtrlFlags flag);
        uint8_t getFlag(StatFlags flag);
        void setFlag(CtrlFlags flag, bool val);
        void setFlag(StatFlags flag, bool val);
        void vblank();
        void oam_scan();
        void hblank();

        void draw();
        void bg_fetch();
        void bg_fetchTileNo();
        void bg_fetchTileDataHi();
        void bg_fetchTileDataLo();
        void bg_fetchPush();
        void sprite_fetch();
        void sprite_fetchTileNo();
        void sprite_fetchTileDataHi();
        void sprite_fetchTileDataLo();
        void sprite_fetchPush();

        Bus* bus;

};

#endif
