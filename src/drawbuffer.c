#include <SDL.h>
#include "drawbuffer.h"
#include "memalloc.h"


/* Draw buffer deals a lot with SDL_Surface's, so it's in charge of keeping 
 * track of the colors: */
Uint32 color_dirt_hi;
Uint32 color_dirt_lo;
Uint32 color_rock;
Uint32 color_fire_hot;
Uint32 color_fire_cold;
Uint32 color_blank;
Uint32 color_bg;
Uint32 color_bg_dot;
Uint32 color_status_bg;
Uint32 color_status_energy;
Uint32 color_status_health;
Uint32 color_primary[8];
Uint32 color_tank[8][3];


/* Run this function after SDL initializes, and after you change bit depths: */
void drawbuffer_refresh_colors() {
	SDL_Surface *s;
	
	s = SDL_GetVideoSurface();
	
	color_dirt_hi       = SDL_MapRGB(s->format, 0xc3, 0x79, 0x30);
	color_dirt_lo       = SDL_MapRGB(s->format, 0xba, 0x59, 0x04);
	color_rock          = SDL_MapRGB(s->format, 0x9a, 0x9a, 0x9a);
	color_fire_hot      = SDL_MapRGB(s->format, 0xff, 0x34, 0x08);
	color_fire_cold     = SDL_MapRGB(s->format, 0xba, 0x00, 0x00);
	color_blank         = SDL_MapRGB(s->format, 0x00, 0x00, 0x00);
	color_bg            = SDL_MapRGB(s->format, 0x00, 0x00, 0x00);
	color_bg_dot        = SDL_MapRGB(s->format, 0x00, 0x00, 0xb6);
	color_status_bg     = SDL_MapRGB(s->format, 0x65, 0x65, 0x65);
	color_status_energy = SDL_MapRGB(s->format, 0xf5, 0xeb, 0x1a);
	color_status_health = SDL_MapRGB(s->format, 0x26, 0xf4, 0xf2);

	/* Primary colors: */
	color_primary[0]    = SDL_MapRGB(s->format, 0x00, 0x00, 0x00);
	color_primary[1]    = SDL_MapRGB(s->format, 0xff, 0x00, 0x00);
	color_primary[2]    = SDL_MapRGB(s->format, 0x00, 0xff, 0x00);
	color_primary[3]    = SDL_MapRGB(s->format, 0xff, 0xff, 0x00);
	color_primary[4]    = SDL_MapRGB(s->format, 0x00, 0x00, 0xff);
	color_primary[5]    = SDL_MapRGB(s->format, 0xff, 0x00, 0xff);
	color_primary[6]    = SDL_MapRGB(s->format, 0x00, 0xff, 0xff);
	color_primary[7]    = SDL_MapRGB(s->format, 0xff, 0xff, 0xff);
	
	/* Blue tank: */
	color_tank[0][0]    = SDL_MapRGB(s->format, 0x2c, 0x2c, 0xff);
	color_tank[0][1]    = SDL_MapRGB(s->format, 0x00, 0x00, 0xb6);
	color_tank[0][2]    = SDL_MapRGB(s->format, 0xf3, 0xeb, 0x1c);
	
	/* Green tank: */
	color_tank[1][0]    = SDL_MapRGB(s->format, 0x00, 0xff, 0x00);
	color_tank[1][1]    = SDL_MapRGB(s->format, 0x00, 0xaa, 0x00);
	color_tank[1][2]    = SDL_MapRGB(s->format, 0xf3, 0xeb, 0x1c);
	
	/* Red tank: */
	color_tank[2][0]    = SDL_MapRGB(s->format, 0xff, 0x00, 0x00);
	color_tank[2][1]    = SDL_MapRGB(s->format, 0xaa, 0x00, 0x00);
	color_tank[2][2]    = SDL_MapRGB(s->format, 0xf3, 0xeb, 0x1c);
	
	/* Pink tank: */
	color_tank[3][0]    = SDL_MapRGB(s->format, 0xff, 0x99, 0x99);
	color_tank[3][1]    = SDL_MapRGB(s->format, 0xaa, 0x44, 0x44);
	color_tank[3][2]    = SDL_MapRGB(s->format, 0xf3, 0xeb, 0x1c);
	
	/* Purple tank: */
	color_tank[4][0]    = SDL_MapRGB(s->format, 0xff, 0x00, 0xff);
	color_tank[4][1]    = SDL_MapRGB(s->format, 0xaa, 0x00, 0xaa);
	color_tank[4][2]    = SDL_MapRGB(s->format, 0xf3, 0xeb, 0x1c);
	
	/* White tank: */
	color_tank[5][0]    = SDL_MapRGB(s->format, 0xee, 0xee, 0xee);
	color_tank[5][1]    = SDL_MapRGB(s->format, 0x99, 0x99, 0x99);
	color_tank[5][2]    = SDL_MapRGB(s->format, 0xf3, 0xeb, 0x1c);
	
	/* Aqua tank: */
	color_tank[6][0]    = SDL_MapRGB(s->format, 0x00, 0xff, 0xff);
	color_tank[6][1]    = SDL_MapRGB(s->format, 0x00, 0xaa, 0xaa);
	color_tank[6][2]    = SDL_MapRGB(s->format, 0xf3, 0xeb, 0x1c);
	
	/* Gray tank: */
	color_tank[7][0]    = SDL_MapRGB(s->format, 0x66, 0x66, 0x66);
	color_tank[7][1]    = SDL_MapRGB(s->format, 0x33, 0x33, 0x33);
	color_tank[7][2]    = SDL_MapRGB(s->format, 0xf3, 0xeb, 0x1c);
}


struct DrawBuffer {
	Uint32 *pixel_data;
	unsigned w, h;
	Uint32 default_color;
};


DrawBuffer *drawbuffer_new(unsigned w, unsigned h) {
	DrawBuffer *out = get_object(DrawBuffer);
	out->pixel_data = get_mem(sizeof(Uint32) * w * h);
	out->w = w; out->h = h;
	out->default_color = 0;
	
	return out;
}

void drawbuffer_destroy(DrawBuffer *b) {
	if(!b) return;
	free_mem(b->pixel_data);
	free_mem(b);
}

void drawbuffer_set_default(DrawBuffer *b, Uint32 color) {
	b->default_color = color;
}

void drawbuffer_set_pixel(DrawBuffer *b, unsigned x, unsigned y, Uint32 color) {
	if(x >= b->w || y >= b->h) return;
	b->pixel_data[ y * b->w + x ] = color;
}

Uint32 drawbuffer_get_pixel(DrawBuffer *b, unsigned x, unsigned y) {
	if(x >= b->w || y >= b->h) return b->default_color;
	return b->pixel_data[ y * b->w + x ];
}

