#include <stdio.h>
#include <time.h>
#include <SDL.h>
#include "screen.h"
#include "tweak.h"
#include "memalloc.h"
#include "level.h"
#include "cavegen.h"
#include "random.h"
#include "drawbuffer.h"
#include "projectile.h"
#include "types.h"
#include "controller.h"
#include "tanklist.h"
#include "guisprites.h"

/* TODO: We should break some of the game-related crap into another file, so
 *       that this file doesn't balloon in size when we introduce the menus, or
 *       the overview map... */

void smart_delay() {
	unsigned cur, next;
	
	/* Get the current time, and the next time: */
	cur  = SDL_GetTicks();
	next = (cur/GAME_FPS_WAIT + 1) * GAME_FPS_WAIT;
	
	/* Wait if we need to: */
	if(cur >= next) return;
	SDL_Delay(next - cur);
}


void main_loop(Screen *s) {
	Level *lvl;
	unsigned frames = 0;
	time_t tiempo, newtiempo;
	SDL_Event e;
	TankList *tl;
	DrawBuffer *b;
	PList *pl;
	Tank *t;
	
	/* Initialize most of the structures: */
	pl  = plist_new();
	b   = drawbuffer_new(1000, 500);
	lvl = level_new(b, 1000, 500);
	tl  = tanklist_new(lvl, pl);
	
	/* Generate our random level: */
	gen_level(lvl);
	level_decorate(lvl);
	level_make_bases(lvl);
	level_dump_bmp(lvl, "debug_start.bmp");
	
	/* Start drawing! */
	drawbuffer_set_default(b, color_rock);
	level_draw_all(lvl, b);
	
	/* Ready the tanks! */
	screen_set_mode_level(s, b);
	t = tanklist_add_tank(tl, 0, level_get_spawn(lvl, 0));
	controller_sdl_attach(t,  SDLK_a, SDLK_d, SDLK_w, SDLK_s, SDLK_LCTRL);
	screen_add_window(s, (SDL_Rect){2, 2, GAME_WIDTH/2-3, GAME_HEIGHT-6-STATUS_HEIGHT }, t);
	screen_add_status(s, (SDL_Rect){3, GAME_HEIGHT - 2 - STATUS_HEIGHT, GAME_WIDTH/2-5-2, STATUS_HEIGHT}, t, 0);
	
	/* Load up two controllable tanks: */
	t = tanklist_add_tank(tl, 1, level_get_spawn(lvl, 1));
	/*controller_sdl_attach(t,  SDLK_LEFT, SDLK_RIGHT, SDLK_UP, SDLK_DOWN, SDLK_SLASH);*/
	controller_twitch_attach(t); /* << Attach a twitch to a camera tank, so we can see if they're getting smarter... */
	screen_add_window(s, (SDL_Rect){GAME_WIDTH/2+1, 2, GAME_WIDTH/2-3, GAME_HEIGHT-6-STATUS_HEIGHT }, t);
	screen_add_status(s, (SDL_Rect){GAME_WIDTH/2+2+2, GAME_HEIGHT - 2 - STATUS_HEIGHT, GAME_WIDTH/2-5-3, STATUS_HEIGHT}, t, 1);

	/* Add the GUI bitmaps: */
	screen_add_bitmap(s, (SDL_Rect){GAME_WIDTH/2-2, GAME_HEIGHT - 2 - STATUS_HEIGHT    , 4, 5}, GUI_ENERGY, &color_status_energy);
	screen_add_bitmap(s, (SDL_Rect){GAME_WIDTH/2-2, GAME_HEIGHT - 2 - STATUS_HEIGHT + 6, 4, 5}, GUI_HEALTH, &color_status_health);
	
	/*screen_add_status(s, (SDL_Rect){}, t, 1);*/
	
	/* Fill up the rest of the slots with Twitches: */
	t = tanklist_add_tank(tl, 2, level_get_spawn(lvl, 2));
	controller_twitch_attach(t);
	t = tanklist_add_tank(tl, 3, level_get_spawn(lvl, 3));
	controller_twitch_attach(t);
	t = tanklist_add_tank(tl, 4, level_get_spawn(lvl, 4));
	controller_twitch_attach(t);
	t = tanklist_add_tank(tl, 5, level_get_spawn(lvl, 5));
	controller_twitch_attach(t);
	t = tanklist_add_tank(tl, 6, level_get_spawn(lvl, 6));
	controller_twitch_attach(t);
	t = tanklist_add_tank(tl, 7, level_get_spawn(lvl, 7));
	controller_twitch_attach(t);
	
	tiempo = time(NULL);
	while(1) {
		
		/* Handle all queued events: */
		while( SDL_PollEvent(&e) ) {
			switch(e.type) {
				/* The user resized the window: */
				case SDL_VIDEORESIZE:
					screen_resize(s, e.resize.w, e.resize.h);
					break;
				
				/* Program is trying to exit: */
				case SDL_QUIT:
					level_dump_bmp(lvl, "debug_end.bmp");
					drawbuffer_destroy(b);
					plist_destroy(pl);
					tanklist_destroy(tl);
					level_destroy(lvl);
					return;
				
				/* Else, ignore: */
				default: ;
			}
		}
		
		/* Clear everything: */
		tanklist_map(tl, tank_clear(t, b));
		plist_clear(pl, b);

		/* Charge a small bit of energy for life: */
		tanklist_map(tl, tank_alter_energy(t, TANK_IDLE_COST));

		/* See if we need to be healed: */
		tanklist_map(tl, tank_try_base_heal(t));
		
		/* Move everything: */
		plist_step(pl, lvl, tl);
		tanklist_map(tl, tank_move(t, tl));
		
		/* Draw everything: */
		plist_draw(pl, b);
		tanklist_map(tl, tank_draw(t, b));
		
		/* Flip buffers: */
		screen_flip(s);
		smart_delay();
		
		/* FPS stuff: */
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
}

	
int main(int argc, char *argv[]) {
	char text[1024];
	Screen *s;
	
	if(argc == 1)
		rand_seed();
	else
		srand(atoi(argv[1]));
	
	if(SDL_Init(SDL_INIT_EVERYTHING)<0) {
		printf("Failed to initialize SDL: %s\n", SDL_GetError());
		return 1;
	}
	
	/* New windowed screen: */
	s = screen_new(0);
	
	/* Dump out the current graphics driver, just for kicks: */
	SDL_VideoDriverName( text, sizeof(text) );
	printf("Using video driver: %s\n", text);
	
	main_loop(s);
	
	screen_destroy(s);
	
	SDL_Quit();
	print_mem_stats();
	return 0;
}
