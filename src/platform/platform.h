
/* █▀█ █░░ ▄▀█ ▀█▀ █▀▀ █▀█ █▀█ █▀▄▀█ */
/* █▀▀ █▄▄ █▀█ ░█░ █▀░ █▄█ █▀▄ █░▀░█ */

/* Platforms: Linux (currently) and Raspberry Pi (future) */

// eventually this #define will be moved to the makefile,
// but idk how to do that yet lol
#define PLATFORM_LINUX

#ifdef PLATFORM_LINUX
  #include "linux/display.h"
#endif

#ifdef PLATFORM_RPIZERO
#endif
