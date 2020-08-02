// SDL stuff
#include <SDL2/SDL.h>
#include <iostream>

SDL_Window* gWindow = NULL;             // The window we'll be rendering to
SDL_Surface* gScreenSurface = NULL;     // The surface contained by the window
SDL_Surface* gSplash = NULL;        // The image we will load and show on the screen
const int SCREEN_WIDTH = 710;
const int SCREEN_HEIGHT = 599;

// Start SDL and create window
bool SDL_init();
bool SDL_init()
{
    //Initialization flag
    bool success = true;

    //Initialize SDL
    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        std::cout << "SDL could not initialize!" << SDL_GetError() << std::endl;
        success = false;
    } else {
        //Create window
        gWindow = SDL_CreateWindow( "Amphy", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if( gWindow == NULL ) {
            std::cout << "Window could not be created!" << SDL_GetError() << std::endl;
            success = false;
        } else {
            //Get window surface
            gScreenSurface = SDL_GetWindowSurface( gWindow );
        }
    }

    return success;
}

// Load images
bool SDL_loadmedia();
bool SDL_loadMedia()
{
    //Loading success flag
    bool success = true;

    //Load splash image
    gSplash = SDL_LoadBMP( "../img/construction.bmp" );
    if ( gSplash == NULL ) {
        std::cout << "Unable to load image" << SDL_GetError() << std::endl;
        success = false;
    }

    return success;
}

// Frees media; shut down SDL
void SDL_close();
void SDL_close()
{
    //Deallocate surface
    SDL_FreeSurface( gSplash );
    gSplash = NULL;

    //Destroy window
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;

    //Quit SDL subsystems
    SDL_Quit();
}

