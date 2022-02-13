
#include <SDL2/SDL.h>
#include <stdio.h>
#include <iostream>
#include <fstream>

class Window
{
    public:
        //The window we'll be rendering to
        SDL_Window* gWindow = NULL;
            
        //The surface contained by the window
        SDL_Surface* gScreenSurface = NULL;

        //The image we will load and show on the screen
        SDL_Surface* gHelloWorld = NULL;

        //Screen dimension constants
        const int SCREEN_WIDTH = 710;
        const int SCREEN_HEIGHT = 599;

    public:
        bool init();
        bool loadMedia();
        void close();
        void applyImg();
};