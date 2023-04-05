
#include <SDL2/SDL.h>
#include <stdio.h>
#include <fstream>

#include "display.h"
#include "../../ppu.h"
#include "../../common.h"

/* @Function Display::init()
 * @brief Initialize SDL window. */
bool Display::Init() {
  bool status = EXIT_SUCCESS;

  if( SDL_Init( SDL_INIT_VIDEO ) >= 0 ) {
    SDL_CreateWindowAndRenderer(SCREEN_WIDTH * 3,
                                SCREEN_HEIGHT * 3,
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

  SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

  if ( status == EXIT_SUCCESS ) {
    // LoadSplash();
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

  // Wait
  SDL_Delay(500);
}

/* @Function Display::RenderPixel()
 * @brief Renders a pixel to the screen. */
void Display::DrawPixel(u16 x, u16 y, Color * c) {
  SDL_SetRenderDrawColor(renderer, c->r, c->g, c->b, 0xFF);
  SDL_RenderDrawPoint(renderer, x, y); 
}

void Display::Render() {
  cleared = false;
  SDL_RenderPresent(renderer);
}

/* @Function Display::HandleEvent */
void Display::HandleEvent() {
  while (SDL_PollEvent(&e) != 0) {
    u8 key, keyType;

    if (e.type == SDL_QUIT) {
      amphy_quit = true;
    }

    else if (e.type == SDL_KEYDOWN) {

      switch (e.key.keysym.sym) {
        case SDLK_RIGHT:
          cpu->Key_Down(KEYTYPE_DIR, KEY_R_A);
          break;
        case SDLK_d:
          cpu->Key_Down(KEYTYPE_ACT, KEY_R_A);
          break;
        
        case SDLK_LEFT:
          cpu->Key_Down(KEYTYPE_DIR, KEY_L_B);
          break;
        case SDLK_f:
          cpu->Key_Down(KEYTYPE_ACT, KEY_L_B);
          break;
        
        case SDLK_UP:
          cpu->Key_Down(KEYTYPE_DIR, KEY_UP_SEL);
          break;
        case SDLK_RSHIFT:
          cpu->Key_Down(KEYTYPE_ACT, KEY_UP_SEL);
          break;
        
        case SDLK_DOWN:
          cpu->Key_Down(KEYTYPE_DIR, KEY_DW_START);
          break;
        case SDLK_RETURN:
          cpu->Key_Down(KEYTYPE_ACT, KEY_DW_START);
          break;
        
        default: break;
      }
    }

    else if (e.type == SDL_KEYUP) {
      switch (e.key.keysym.sym) {
        case SDLK_RIGHT:
          cpu->Key_Up(KEYTYPE_DIR, KEY_R_A);
          break;
        case SDLK_d:
          cpu->Key_Up(KEYTYPE_ACT, KEY_R_A);
          break;
        
        case SDLK_LEFT:
          cpu->Key_Up(KEYTYPE_DIR, KEY_L_B);
          break;
        case SDLK_f:
          cpu->Key_Up(KEYTYPE_ACT, KEY_L_B);
          break;
        
        case SDLK_UP:
          cpu->Key_Up(KEYTYPE_DIR, KEY_UP_SEL);
          break;
        case SDLK_RSHIFT:
          cpu->Key_Up(KEYTYPE_ACT, KEY_UP_SEL);
          break;
        
        case SDLK_DOWN:
          cpu->Key_Up(KEYTYPE_DIR, KEY_DW_START);
          break;
        case SDLK_RETURN:
          cpu->Key_Up(KEYTYPE_ACT, KEY_DW_START);
          break;
        
        default: break;
      }
    }
  }
}

void Display::Clear(Color * c) {
  cleared = true;
  SDL_SetRenderDrawColor(renderer, c->r, c->g, c->b, 0xFF);
  SDL_RenderClear(renderer);
}
