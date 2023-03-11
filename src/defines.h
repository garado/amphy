
/* █▀▄ █▀▀ █▀▀ █ █▄░█ █▀▀ █▀ */ 
/* █▄▀ ██▄ █▀░ █ █░▀█ ██▄ ▄█ */ 

#ifndef DEFINES_H
#define DEFINES_H

#define FAILURE 1
#define SUCCESS 0

// Registers (required by cpu and bus)
#define DIV  0xFF04 // Divider register
#define TIMA 0xFF05 // Timer counter
#define TMA  0xFF06 // Timer modulo
#define TAC  0xFF07 // Timer control
#define INTE 0xFFFF // Interrupt enable
#define INTF 0xFF0F // Interrupt flag
#define JOYP 0xFF00 // Joypad
#define SERB 0xFF01 // Serial transfer data
#define SERC 0xFF02 // Serial transfer control

#endif
