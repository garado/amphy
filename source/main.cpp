#include "platform.h"
#include <iostream>

int main(int arg, char *argv[])
{
    if ( !SDL_init() ) {
        std::cout << "Oh no! Failed to intialize :(" << std::endl;
    } else {
        if ( !SDL_loadMedia() ) {
            std::cout << "Oh no! Failed to load media :(" << std::endl;
        } else {
            SDL_BlitSurface( gSplash, NULL, gScreenSurface, NULL );
            SDL_UpdateWindowSurface( gWindow );
            SDL_Delay( 2000 );
        }
        SDL_close();
        return 0;
    }
}