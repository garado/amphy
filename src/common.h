
#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

#define FAILURE 1
#define SUCCESS 0

#define FALSE 0
#define TRUE !FALSE

typedef uint8_t u8;
typedef uint16_t u16;

/* █▀▄▀█ ▄▀█ █▀▀ █▀█ █▀█ █▀ */
/* █░▀░█ █▀█ █▄▄ █▀▄ █▄█ ▄█ */

#define BIT_TOGGLE(byte,nbit) ((byte) ^ (1 << nbit))
#define BIT_SET(byte,nbit)    ((byte) | (1 << nbit)) 
#define BIT_TEST(byte,nbit)   ((byte) & (1 << nbit))
#define BIT_CLEAR(byte,nbit)  ((byte) & ~(1 << nbit)) 
#define BIT_GET(byte,nbit)    ((byte >> nbit) & 1)

/* █▀█ █▀▀ █▀▀ █ █▀ ▀█▀ █▀▀ █▀█ █▀ */
/* █▀▄ ██▄ █▄█ █ ▄█ ░█░ ██▄ █▀▄ ▄█ */

/* IO registers */
#define JOYP 0xFF00 // P1/JOYP: Joypad
#define JOYP_SEL_ACT_VAL 0b00010000
#define JOYP_SEL_DIR_VAL 0b00100000
#define JOYP_SEL_NIL_VAL 0b01100000
#define JOYP_SEL_BOTH_VAL 0b00000000

#define SERB 0xFF01 // Serial byte
#define SERC 0xFF02 // Serial control
#define DIV  0xFF04 // Clock divider
#define TIMA 0xFF05 // Timer value
#define TMA  0xFF06 // Timer reload/modulo
#define TAC  0xFF07 // Timer control
#define BOOT 0xFF50 // Boot ROM control


/* Interrupts */
// IF: Interrupt requests
#define INTF 0xFF0F
#define INTF_VBLANK_IRQ 0
#define INTF_STAT_IRQ 1
#define INTF_TMR_IRQ 2
#define INTF_SRL_IRQ 3
#define INTF_JOYP_IRQ 4

// IE: Interrupt enable
#define INTE 0xFFFF
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

// LCDC: LCD control
#define LCDC 0xFF40
#define LCDC_EN 7
#define LCDC_WIN_TMAP 6
#define LCDC_WIN_EN 5
#define LCDC_BGW_ADDR_MODE 4
#define LCDC_BG_TMAP 3
#define LCDC_OBJ_SIZE 2
#define LCDC_OBJ_EN 1
#define LCDC_BG_EN 0

// STAT: LCD status
#define STAT 0xFF41
#define STAT_LYC_INTR 6
#define STAT_OAM_INTR 5
#define STAT_VBLANK_INTR 4
#define STAT_HBLANK_INTR 3
#define STAT_LYC_FLAG 2 

// Misc PPU registers
#define SCY  0xFF42 // Background vert. scroll
#define SCX  0xFF43 // SCX: Background horiz. scroll
#define LY   0xFF44 // LCD Y coordinate
#define LYC  0xFF45 // LCD Y compare
#define DMA  0xFF46 // OAM DMA source address
#define BGP  0xFF47 // Background palette
#define OBP0 0xFF48 // OBJ palette 0
#define OBP1 0xFF49 // OBJ palette 1
#define WY   0xFF4A // Window Y coord
#define WX   0xFF4B // Window X coord

/* █▄▀ █▀▀ █▄█ █▀ */
/* █░█ ██▄ ░█░ ▄█ */

enum KeyType {
  KEYTYPE_DIR,
  KEYTYPE_ACT,
};

enum Keys {
  KEY_R_A,
  KEY_L_B,
  KEY_UP_SEL,
  KEY_DW_START,
};


/* █▀▀ █▀█ ▄▀█ █▀█ █░█ █ █▀▀ █▀ */
/* █▄█ █▀▄ █▀█ █▀▀ █▀█ █ █▄▄ ▄█ */

typedef struct Color {
  u16 r;
  u16 g; 
  u16 b;
  u16 a;
} Color;

#endif
