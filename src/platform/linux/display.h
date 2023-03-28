
/* █▀ █▀▄ █░░ */
/* ▄█ █▄▀ █▄▄ */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "../../defines.h"

// Size of SDL screen
#define GAMEBOY_WIDTH 160
#define GAMEBODY_HEIGHT 144

#define SCALE_FACTOR 1

// #define SCREEN_WIDTH GAMEBOY_WIDTH * SCALE_FACTOR
// #define SCREEN_HEIGHT GAMEBODY_HEIGHT * SCALE_FACTOR
#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144


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

  private:
    bool LoadSplash();
    void ApplyImg();

  public:
    bool Init();
    void Close();
    void HandleEvent();
    void DrawPixel(uint16_t x, uint16_t y, Color * c);
    void Render();
};

#endif
