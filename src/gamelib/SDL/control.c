#include <stdio.h>
#include <SDL.h>
#include "../../tank.h"
#include "../../tweak.h"

/* Only used in this function, so screw the header file: */
void controller_sdl_attach( Tank *, SDLKey, SDLKey, SDLKey, SDLKey, SDLKey);


/* Set up SDL: */
int gamelib_init() {
	char text[1024];
	
	if( SDL_Init(SDL_INIT_EVERYTHING)<0 ) {
		printf("Failed to initialize SDL: %s\n", SDL_GetError());
		return 1;
	}
	
	/* Dump out the current graphics driver, just for kicks: */
	SDL_VideoDriverName( text, sizeof(text) );
	printf("Using video driver: %s\n", text);
	
	return 0;
}

/* Frees stuff up: */
int gamelib_exit() {
	SDL_Quit();
	return 0;
}

/* Waits long enough to maintain a consistent FPS: */
void gamelib_smart_wait() {
	unsigned cur, next;
	
	/* Get the current time, and the next time: */
	cur  = SDL_GetTicks();
	next = (cur/GAME_FPS_WAIT + 1) * GAME_FPS_WAIT;
	
	/* Wait if we need to: */
	if(cur >= next) return;
	SDL_Delay(next - cur);
}

/*
void gamelib_handle_fps() {
	frames += 1;
	newtiempo = time(NULL);
	if(newtiempo != tiempo) {
		char buffer[50];
		sprintf(buffer, "%s %s (%u fps)", WINDOW_TITLE, VERSION, frames);
		SDL_WM_SetCaption(buffer, buffer);
		frames = 0;
		tiempo = newtiempo;
	}
}
*/

/* All of this backend's capabilities: */
int gamelib_get_max_players()    { return 2; }
int gamelib_get_can_resize()     { return 1; }
int gamelib_get_can_fullscreen() { return 1; }
int gamelib_get_can_window()     { return 1; }
int gamelib_get_target_fps()     { return GAME_FPS; }


/* TODO: De-uglyify this: */
int gamelib_tank_attach(Tank *t, int tank_num, int num_players) {
	if(num_players == 1 && tank_num == 0)
		controller_sdl_attach(t, SDLK_LEFT, SDLK_RIGHT, SDLK_UP, SDLK_DOWN, SDLK_LCTRL);
	
	else if(num_players == 2) {
		if     (tank_num == 0) controller_sdl_attach(t, SDLK_a, SDLK_d, SDLK_w, SDLK_s, SDLK_LCTRL);
		else if(tank_num == 1) controller_sdl_attach(t, SDLK_LEFT, SDLK_RIGHT, SDLK_UP, SDLK_DOWN, SDLK_SLASH);
		else return 1;
	}
	
	else return 1;
	
	return 0;
}

