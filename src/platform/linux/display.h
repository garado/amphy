
/* █▀ █▀▄ █░░ */
/* ▄█ █▄▀ █▄▄ */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <stdio.h>
#include "../../common.h"

// Size of SDL screen
#define GAMEBOY_WIDTH 160
#define GAMEBODY_HEIGHT 144

#define SCALE_FACTOR 1

// #define SCREEN_WIDTH GAMEBOY_WIDTH * SCALE_FACTOR
// #define SCREEN_HEIGHT GAMEBODY_HEIGHT * SCALE_FACTOR
#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144

enum KeyPressSurfaces {
  KEY_PRESS_SURFACE_DEFAULT,
  KEY_PRESS_SURFACE_UP,
  KEY_PRESS_SURFACE_DOWN,
  KEY_PRESS_SURFACE_LEFT,
  KEY_PRESS_SURFACE_RIGHT,
  KEY_PRESS_SURFACE_TOTAL
};

class Cpu;

class Display
{
  public:
    // The window we'll be rendering to
    SDL_Window* gWindow = NULL;

    // The surface contained by the window
    SDL_Surface* gScreenSurface = NULL;

    // The image we will load and show on the screen
    SDL_Surface* gSurface = NULL;

    SDL_Renderer* renderer = NULL;

    bool amphy_quit = false;
    SDL_Event e;

    Cpu * cpu;

    bool cleared = false;

  private:
    bool LoadSplash();
    void ApplyImg();

  public:
    bool Init();
    void Close();
    void HandleEvent();
    void DrawPixel(u16 x, u16 y, Color * c);
    void Render();
    void Clear(Color * c);
};

#endif
