
#include <SDL2/SDL.h>
#include <stdio.h>
#include <iostream>
#include <fstream>

#include "display.h"
#include "../../ppu/ppu.h"
#include "../../defines.h"

/* @Function Display::init()
 * @brief Initialize SDL window. */
bool Display::Init() {
  bool status = EXIT_SUCCESS;

  if( SDL_Init( SDL_INIT_VIDEO ) >= 0 ) {
    SDL_CreateWindowAndRenderer(SCREEN_WIDTH,
                                SCREEN_HEIGHT,
                                0,
                                &gWindow, &renderer);
    if (gWindow != NULL) {
      gScreenSurface = SDL_GetWindowSurface( gWindow );
      SDL_SetWindowTitle(gWindow, "Amphy");
    } else {
      printf("Display could not be created! SDL_Error: %s", SDL_GetError());
      status = EXIT_FAILURE;
    }
  } else {
    printf("SDL could not initialize! SDL_Error: %s", SDL_GetError()),
    status = EXIT_FAILURE;
  }

  if ( status == EXIT_SUCCESS ) {
    LoadSplash();
  } else {
    printf("SDL: Failed to initialize.");
  }

  return status;
}

/* @Function Display::LoadSplash
 * @brief Show Cute splash screen */
bool Display::LoadSplash()
{
  char * splash = strcat(SDL_GetBasePath(), "img/construction.bmp");
  gSurface = SDL_LoadBMP( splash );

  if ( gSurface != NULL ) {
    ApplyImg();
  } else {
    printf( "Unable to load image %s! SDL Error: %s\n", splash, SDL_GetError() );
    Close();
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

/* @Function Display::Close()
 * @brief Frees media and shuts down SDL. */
void Display::Close()
{
  SDL_FreeSurface( gSurface );
  gSurface = NULL;

  SDL_DestroyWindow( gWindow );
  gWindow = NULL;

  SDL_Quit();
}

/* @Function Display::ApplyImg
 * Apply... the image */
void Display::ApplyImg() {
  // Apply the image
  SDL_BlitSurface(gSurface, NULL, gScreenSurface, NULL);

  // Update the surface
  SDL_UpdateWindowSurface(gWindow);

  // Wait two seconds
  SDL_Delay(2000);
}

/* @Function Display::RenderPixel()
 * @param x x-coordinate of Gameboy
 * @param y y-coordinate of Gameboy
 * @param color color of pixel
 * @brief Renders a pixel to the screen. */
void Display::DrawPixel(uint16_t x, uint16_t y, Color * c) {
  SDL_SetRenderDrawColor(renderer, c->r, c->g, c->b, 0xFF);
  SDL_RenderDrawPoint(renderer, x, y); 
}

void Display::Render()
{
  SDL_RenderPresent(renderer);
}

/* @Function Display::HandleEvent
 * @brief Handles SDL events. */
void Display::HandleEvent() {
  while (SDL_PollEvent(&e) != 0) {
    if (e.type == SDL_QUIT) {
      amphy_quit = true;
    }
  }
}
