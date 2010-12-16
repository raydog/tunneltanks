#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include <SDL.h>
#include "tank.h"

/* TODO: This SDL-laced sucker is getting moved to gamelib... eventually... */
void controller_sdl_attach(Tank *, SDLKey l, SDLKey r, SDLKey u, SDLKey d, SDLKey s) ;
void controller_twitch_attach(Tank *t) ;

#endif /* _CONTROLLER_H_ */

