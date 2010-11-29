#ifndef _CONTROLLER_SDL_H_
#define _CONTROLLER_SDL_H_

#include <SDL.h>
#include "tank.h"

void controller_sdl_attach(Tank *, SDLKey l, SDLKey r, SDLKey u, SDLKey d, SDLKey s) ;
void controller_twitch_attach(Tank *t) ;

#endif /* _CONTROLLER_SDL_H_ */

