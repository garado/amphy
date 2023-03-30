
/* █░█ ▀█▀ █ █░░ █▀ */
/* █▄█ ░█░ █ █▄▄ ▄█ */

#include "utils.h"
#include "bus.h"
#include <unistd.h>

/* @Function ParseFlags
 * @brief Parse command line flags */
void ParseFlags(int argc, char* argv[], Cpu* cpu) {
  /* Flags
   *  -d debug mode (step)
   *  -g gbdoc mode */
  int c;
  while ((c = getopt(argc, argv, ":dg")) != -1) {
    switch (c) {
      case 'g': cpu->gbdoc = true; break;
      case 'd': cpu->step  = true; break;
      default:  break;
    }
  }
}

