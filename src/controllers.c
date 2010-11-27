#include <SDL.h>
#include "controllers.h"
#include "tank.h"
#include "projectile.h"
#include "tweak.h"
#include "memalloc.h"
#include "random.h"


/* The SDL-based keyboard controller: */
typedef struct SDLKPrivateData {
	SDLKey left, right, up, down, shoot;
} SDLKPrivateData;

static void sdlk_controller(Tank *t, void *d, Sint8 *vx, Sint8 *vy, Uint8 *s) {
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



/* Our first AI: Twitch! (note: the 'I' in 'AI' is being used VERY loosely) */

typedef struct TwitchPrivateData {
	Sint8 vx, vy, s;
	Uint8 time_to_change;
} TwitchPrivateData;

static void twitch_controller(Tank *t, void *d, Sint8 *vx, Sint8 *vy, Uint8 *s) {
	TwitchPrivateData *data = d;
	
	if(!data->time_to_change) {
		data->time_to_change = rand_int(10, 30);
		data->vx = rand_int(0,2) - 1;
		data->vy = rand_int(0,2) - 1;
		data->s  = rand_bool(300);
	}
	
	data->time_to_change--;
	*vx = data->vx;
	*vy = data->vy;
	*s  = data->s;
}

void controller_twitch_attach( Tank *t ) {
	TwitchPrivateData *data = get_object(TwitchPrivateData);
	data->time_to_change = 0;
	
	tank_set_controller(t, twitch_controller, data);
}

