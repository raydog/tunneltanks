#include <stdlib.h>
#include <jni.h>

#include <gamelib.h>
#include <types.h>
#include <tank.h>
#include <tweak.h>
#include <game.h>

#include "androiddata.h"
#include "require_android.h"


/* Game startup, and shutdown: */
int gamelib_init() {
	_DATA.gd = game_new();
	
	game_set_level_gen(_DATA.gd, "maze");
	game_finalize(_DATA.gd);
	
	return 0;
}

int gamelib_exit() {
	game_free(_DATA.gd);
	return 0;
}


/* All of the Android limits/capabilities: */
int gamelib_get_max_players()    { return 1; }
int gamelib_get_can_resize()     { return 0; }
int gamelib_get_can_fullscreen() { return 1; }
int gamelib_get_can_window()     { return 0; }
int gamelib_get_target_fps()     { return GAME_FPS; } /* << TODO: Remove this? */

/* The C code doesn't maintain the main loop in the Android port, so we don't
 * need to implement this: */
void gamelib_main_loop(draw_func func, void *data) {}

/* This lets you attach controllers to a tank: */
int gamelib_tank_attach(Tank *t, int tank_num, int num_players) {
	return 0;
}

/* A user can't ask for a resize (so gamelib_get_can_resize() returns 0) but the
 * bitmap can be resized by Android, so we will still issue RESIZE events: */
EventType gamelib_event_get_type() { return GAME_EVENT_NONE; }
Rect      gamelib_event_resize_get_size() { return _DATA.prev; }
void      gamelib_event_done() {}

/* We need to be able to switch resolutions: */
int  gamelib_set_fullscreen() {return 0; }
int  gamelib_set_window(unsigned w, unsigned h) { return 0; }
Rect gamelib_get_resolution() { return RECT(0,0,0,0); }
int  gamelib_get_fullscreen() { return 1; }

/* We need a way to draw: */
/* TODO: We are wasting a lot of time on all the range checking. We need to
 *       change the API so that the resource is reserved until we're done. That
 *       way, range-checking happens on a per-frame basis, and not a per-pixel
 *       basis. This will speed up SDL too... */
int  gamelib_draw_box(Rect *rect, Color c) { return 0; }

/* Mobile devices have limited storage. Don't waste it with huge bmps: */
BMPFile *gamelib_bmp_new      (unsigned width, unsigned height) {return NULL;}
void     gamelib_bmp_set_pixel(BMPFile *f, unsigned x, unsigned y, Color c) {}
void     gamelib_bmp_finalize (BMPFile *f, char *filename) {}

