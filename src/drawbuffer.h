#ifndef _DRAW_BUFFER_H_
#define _DRAW_BUFFER_H_

typedef struct DrawBuffer DrawBuffer;

#include <SDL.h>

extern Uint32 color_dirt_hi;
extern Uint32 color_dirt_lo;
extern Uint32 color_rock;
extern Uint32 color_fire_hot;
extern Uint32 color_fire_cold;
extern Uint32 color_blank;
extern Uint32 color_bg;
extern Uint32 color_bg_dot;
extern Uint32 color_status_bg;
extern Uint32 color_status_energy;
extern Uint32 color_status_health;
extern Uint32 color_primary[8];
extern Uint32 color_tank[8][3];

DrawBuffer *drawbuffer_new(unsigned w, unsigned h) ;
void drawbuffer_destroy(DrawBuffer *b) ;

void   drawbuffer_set_default(DrawBuffer *b, Uint32 color) ;
void   drawbuffer_set_pixel(DrawBuffer *b, unsigned x, unsigned y, Uint32 color) ;
Uint32 drawbuffer_get_pixel(DrawBuffer *b, unsigned x, unsigned y) ;

void drawbuffer_refresh_colors();

#endif /* _DRAW_BUFFER_H_ */

