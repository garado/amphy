
/* █▀█ █░░ ▄▀█ ▀█▀ █▀▀ █▀█ █▀█ █▀▄▀█ */
/* █▀▀ █▄▄ █▀█ ░█░ █▀░ █▄█ █▀▄ █░▀░█ */

// eventually this #define will be moved to the makefile,
// but idk how to do that yet lol
#define PLATFORM_LINUX

#ifdef PLATFORM_LINUX
  #include "linux/display.h"
#endif

#ifdef PLATFORM_ESP32
#endif
