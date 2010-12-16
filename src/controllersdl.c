#include <SDL.h>
#include "controller.h"
#include "tank.h"
#include "memalloc.h"

/* The SDL-based keyboard controller: */
typedef struct SDLKPrivateData {
	SDLKey left, right, up, down, shoot;
} SDLKPrivateData;

static void sdlk_controller(PublicTankInfo *i, void *d, int *vx, int *vy, unsigned *s) {
	SDLKPrivateData *data = d;
	Uint8 *keys = SDL_GetKeyState( NULL );
	
	*vx = keys[data->right] - keys[data->left];
	*vy = keys[data->down]  - keys[data->up];
	*s  = keys[data->shoot];
}

void controller_sdl_attach( Tank *t,
	SDLKey left, SDLKey right, SDLKey up, SDLKey down, SDLKey shoot) {
	
	SDLKPrivateData *data = get_object(SDLKPrivateData);
	*data = (SDLKPrivateData){left, right, up, down, shoot};
	
	tank_set_controller(t, sdlk_controller, data);
}

