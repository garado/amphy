
#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;

typedef struct Color {
  uint16_t r;
  uint16_t g; 
  uint16_t b;
  uint16_t a;
} Color;

#define FAILURE 1
#define SUCCESS 0


/* █▀█ █▀▀ █▀▀ █ █▀ ▀█▀ █▀▀ █▀█ █▀ */
/* █▀▄ ██▄ █▄█ █ ▄█ ░█░ ██▄ █▀▄ ▄█ */

/* IO registers */
#define JOYP 0xFF00 // P1/JOYP: Joypad
#define SERB 0xFF01 // SB: Serial byte
#define SERC 0xFF02 // SC: Serial control
#define DIV  0xFF04 // DIV: Clock divider
#define TIMA 0xFF05 // TIMA: Timer value
#define TMA  0xFF06 // TMA: Timer reload/modulo
#define TAC  0xFF07 // TAC: Timer control

/* Interrupts */
#define INTF 0xFF0F // IF: Interrupt requests
#define INTF_VBLANK_IRQ 0
#define INTF_STAT_IRQ 1
#define INTF_TMR_IRQ 2
#define INTF_SRL_IRQ 3
#define INTF_JOYP_IRQ 4

#define INTE 0xFFFF // IE: Interrupt enable
#define INTE_VBLANK_IE 0
#define INTE_STAT_IE 1
#define INTE_TMR_IE 2
#define INTE_SRL_IE 3
#define INTE_JOYP_IE 4

#define NR10 0xFF10 // NR10: Audio channel 1 sweep
#define NR11 0xFF11 // NR11: Audio channel 1 sound length/wave duty
#define NR12 0xFF12 // NR12: Audio channel 1 envelope
#define NR13 0xFF13 // NR13: Audio channel 1 frequency
#define NR14 0xFF14 // NR14: Audio channel 1 control

#define NR21 0xFF16 // NR21: Audio channel 2 sound length/wave duty
#define NR22 0xFF17 // NR22: Audio channel 2 envelope
#define NR23 0xFF18 // NR23: Audio channel 2 frequency
#define NR24 0xFF19 // NR24: Audio channel 2 control

#define NR30 0xFF1A // NR30: Audio channel 3 enable
#define NR31 0xFF1B // NR31: Audio channel 3 sound length
#define NR32 0xFF1C // NR32: Audio channel 3 volume
#define NR33 0xFF1D // NR33: Audio channel 3 frequency
#define NR34 0xFF1E // NR34: Audio channel 3 control

#define NR41 0xFF20 // NR41: Audio channel 4 sound length
#define NR42 0xFF21 // NR42: Audio channel 4 volume
#define NR43 0xFF22 // NR43: Audio channel 4 frequency
#define NR44 0xFF23 // NR44: Audio channel 4 control

#define NR50 0xFF24 // NR50: Audio output mapping
#define NR51 0xFF25 // NR51: Audio channel mapping
#define NR52 0xFF26 // NR52: Audio channel control

// FF30 – $FF3F: Wave pattern

#define LCDC 0xFF40 // LCDC: LCD control
#define LCDC_EN 7
#define LCDC_WIN_TMAP 6
#define LCDC_WIN_EN 5
#define LCDC_BGW_ADDR_MODE 4
#define LCDC_BG_TMAP 3
#define LCDC_OBJ_SIZE 2
#define LCDC_OBJ_EN 1
#define LCDC_BG_EN 0

#define STAT 0xFF41
#define STAT_LYC_INTR_EN 6
#define STAT_OAM_INTR_EN 5
#define STAT_VBLANK_INTR_EN 4
#define STAT_HBLANK_INTR_EN 3
#define STAT_LYC_FLAG 2 

#define SCY  0xFF42 // SCY: Background vert. scroll
#define SCX  0xFF43 // SCX: Background horiz. scroll
#define LY   0xFF44 // LY: LCD Y coordinate
#define LYC  0xFF45 // LYC: LCD Y compare
#define DMA  0xFF46 // DMA: OAM DMA source address
#define BGP  0xFF47 // BGP: Background palette
#define OBP0 0xFF48 // OBP0: OBJ palette 0
#define OBP1 0xFF49 // OBP1: OBJ palette 1
#define WY   0xFF4A // WY: Window Y coord
#define WX   0xFF4B // WX: Window X coord
#define BOOT 0xFF50 // Boot ROM control

#endif
