# Cinoop
- Create a window
- Read documents to get a good overview
- Registers

# Technical Data
- CPU: 8bit
- Clock speed: 4.194304MHz (4.295454MHz for SGB, max. 8.4MHz for CGB)
- Work RAM: 8K Byte
- Video RAM: 8K Byte
- Screen Size: 2.6"
- Resolution: 160x144 (20x18 tiles)
- Max Sprites: 10 per screen, 40 per line
- Sprite Sizes: 8x8 or 8x16
- Palettes: 1x4 BG, 2x3 OBJ
- Colors: 4 grayshades
- Horiz Sync: 9198 KHz
- Vert Sync: 59.73 Hz
- Sound: 4 channels with stereo sound

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <direct.h>
#include <gl/gl.h>
#include <LDFS.h>

#include "rom.h"
#include "cpu.h"
#include "gpu.h"
#include "interrupts.h"
#include "opengl.h"
#include "debug.h"
#include "keys.h"

#include "main.h"