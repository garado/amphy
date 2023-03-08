
#include <SDL2/SDL.h>
#include <stdio.h>
#include <iostream>
#include <fstream>

#include "win.h"

/* Window::init()
 * Initialize SDL window. */
bool Window::init() {
  bool success = true;

  // Initialize SDL
  if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
    std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
    success = false;
  } else {
    // Create window
    gWindow = SDL_CreateWindow("Amphy", 
                               SDL_WINDOWPOS_UNDEFINED, 
                               SDL_WINDOWPOS_UNDEFINED, 
                               SCREEN_WIDTH, SCREEN_HEIGHT, 
                               SDL_WINDOW_SHOWN);
    if (gWindow == NULL) {
      std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
      success = false;
    } else {
      // Get window surface
      gScreenSurface = SDL_GetWindowSurface( gWindow );
    }
  }

  // Create renderer
  // renderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
  // if (renderer == NULL){
  //   std::cerr << "Oh no! Renderer could not be created. SDL Error: " << SDL_GetError() << std::endl;
  //   success = false;
  // } else {
  //   SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
  // }

  return EXIT_SUCCESS;
}

// Loads media
bool Window::LoadMedia()
{
  // Load splash image
  gHelloWorld = SDL_LoadBMP( "img/construction.bmp" );
  if ( gHelloWorld == NULL ) {
    printf( "Unable to load image %s! SDL Error: %s\n", "02_getting_an_image_on_the_screen/hello_world.bmp", SDL_GetError() );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

// Frees media and shuts down SDL
void Window::close()
{
  // Deallocate surface
  SDL_FreeSurface( gHelloWorld );
  gHelloWorld = NULL;

  // Destroy window
  SDL_DestroyWindow( gWindow );
  gWindow = NULL;

  // Quit SDL subsystems
  SDL_Quit();
}

void Window::ApplyImg() {
  // Apply the image
  SDL_BlitSurface(gHelloWorld, NULL, gScreenSurface, NULL);

  // Update the surface
  SDL_UpdateWindowSurface(gWindow);

  // Wait two seconds
  SDL_Delay(2000);
}

/* Window::RenderPixel()
 * Renders a pixel to the screen. */
void Window::RenderPixel(uint16_t x, uint16_t y, Color color) {
  SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
  SDL_RenderClear(renderer);
  SDL_SetRenderDrawColor(renderer, 0x00,0x00,0x9F,0xFF);
}
