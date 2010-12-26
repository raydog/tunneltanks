#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <jni.h>
#include <android/bitmap.h>

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
int  gamelib_draw_box(Rect *rect, Color c) { 
	register unsigned x, y;
	Rect size;
	void *pixels;
	AndroidBitmapInfo info;
	uint16_t color_data;
	
	/* Try to get them pixels: */
	if(AndroidBitmap_lockPixels(_DATA.env, _DATA.bitmap, &pixels)) {
		fprintf(stderr, "Failed to lock the bitmap's pixel array.");
		exit(1);
	}
	
	/* Let's get some info on those now-locked pixels: */
	if(AndroidBitmap_getInfo(_DATA.env, _DATA.bitmap, &info)) {
		fprintf(stderr, "Failed to get bitmap info before drawing.");
		exit(1);
	}
	
	/* Get the bounds of what we're drawing: */
	if(rect) size = *rect;
	else {
		size.x = size.y = 0;
		size.w = info.width; size.h = info.height;
	}
	
	/* Pack the color info into the color_data array: 
	 * (This bit liberated from the Android NDK plasma example: */
	color_data = (uint16_t)( ((c.r << 8) & 0xf800) | 
	                         ((c.g << 2) & 0x03e0) |
	                         ((c.b >> 3) & 0x001f) );
	
	/* Scan all pixels, and set the ones we need: */
	for(y=0; y<size.h; y++) {
		for(x=0; x<size.w; x++) {
			/* TODO: Would there be a way of doing this more like SDL, so that
			 * we don't force the bitmap into RGB565 format, but instead let the
			 * platform select its favorite pixel format? Think on this... */
			uint16_t *p = &((uint16_t *)pixels)[ y*info.stride + x ];
			memcpy( p, &color_data, 2 );
			
			/* In SDL, it's this. Android will be similar...
			Uint8 *p = &((Uint8*)s->pixels)[ y*s->pitch + x*s->format->BytesPerPixel ];
			memcpy( p, &color_bg_dot, s->format->BytesPerPixel );
			*/
		}
	}
	
	/* Ok, the data is copied in. Release the pixels: */
	if(AndroidBitmap_unlockPixels( _DATA.env, _DATA.bitmap )) {
		fprintf(stderr, "Failed to unlock bitmap.");
		exit(1);
	}
	
	return 0;
}


/* Mobile devices have limited storage. Don't waste it with huge bmps: */
BMPFile *gamelib_bmp_new      (unsigned width, unsigned height) {return NULL;}
void     gamelib_bmp_set_pixel(BMPFile *f, unsigned x, unsigned y, Color c) {}
void     gamelib_bmp_finalize (BMPFile *f, char *filename) {}

