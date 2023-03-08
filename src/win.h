
#include <SDL2/SDL.h>
#include <stdio.h>
#include <iostream>
#include <fstream>

#ifndef WINDOW_H
#define WINDOW_H

class Window
{
  public:
    // The window we'll be rendering to
    SDL_Window* gWindow = NULL;
      
    // The surface contained by the window
    SDL_Surface* gScreenSurface = NULL;

    // The image we will load and show on the screen
    SDL_Surface* gHelloWorld = NULL;

    SDL_Renderer* renderer = NULL;

    // Screen dimension constants
    const int SCREEN_WIDTH = 710;
    const int SCREEN_HEIGHT = 599;

    typedef struct Color {
      uint16_t r;
      uint16_t g; 
      uint16_t b;
      uint16_t a;
    } Color;

  public:
    bool init();
    bool LoadMedia();
    void close();
    void ApplyImg();
    void RenderPixel(uint16_t x, uint16_t y, Color color);
};

#endif
