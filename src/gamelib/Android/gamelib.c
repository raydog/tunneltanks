
#ifndef USE_ANDROID_GAMELIB
	#error "USE_ANDROID_GAMELIB must be defined if this file to be compiled."
#endif

#include <stdlib.h>
#include <jni.h>
#include "../gamelib.h"
#include "../../types.h"
#include "../../tank.h"
#include "../../tweak.h"

/* If the gamelib needs initialization, this'll do it: */
int gamelib_init() {
	return 0;
}

/* If the gamelib needs to free resources before exiting, this'll do it: */
int gamelib_exit() {
	return 0;
}

/* Gives a way to poll the gamelib for the capabilities provided by the
 * underlying system: */
int gamelib_get_max_players()    { return 1; }
int gamelib_get_can_resize()     { return 0; }
int gamelib_get_can_fullscreen() { return 1; }
int gamelib_get_can_window()     { return 0; }
int gamelib_get_target_fps()     { return GAME_FPS; }

/* Some platforms (Android) will be acting as the game loop, so the game loop
 * needs to happen in the gamelib: */
void gamelib_main_loop(draw_func func, void *data) {

}

/* This lets main() attach controllers to a tank: */
int gamelib_tank_attach(Tank *t, int tank_num, int num_players) {
	return 0;
}

EventType gamelib_event_get_type() { return GAME_EVENT_NONE; }
Rect      gamelib_event_resize_get_size() { return RECT(0,0,0,0); }
void      gamelib_event_done() {}

/* We need to be able to switch resolutions: */
int  gamelib_set_fullscreen() {return 0; }
int  gamelib_set_window(unsigned w, unsigned h) { return 0; }
Rect gamelib_get_resolution() { return RECT(0,0,0,0); }
int  gamelib_get_fullscreen() { return 1; }

/* We need a way to draw: */
int  gamelib_draw_box(Rect *rect, Color c) { return 0; }

/* Now, for a way to draw a bitmap, if the platform wants to... */
BMPFile *gamelib_bmp_new      (unsigned width, unsigned height) { return NULL; }
void     gamelib_bmp_set_pixel(BMPFile *f, unsigned x, unsigned y, Color c) {}
void     gamelib_bmp_finalize (BMPFile *f, char *filename) {}

