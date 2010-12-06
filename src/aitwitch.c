#include <SDL.h>
#include <math.h>
#include "controller.h"
#include "tank.h"
#include "memalloc.h"
#include "random.h"
#include "tweak.h"


/* Used when seeking a base entrance: */
#define OUTSIDE (BASE_SIZE/2 + 5)


/* Our first AI: Twitch! (note: the 'I' in 'AI' is being used VERY loosely) */

/* The different Twitch travel modes: */
typedef enum TwitchMode {
	TWITCH_EXIT_UP,   /* Leave the base in an upward direction. */
	TWITCH_EXIT_DOWN, /* Leave the base in a downward direction. */
	TWITCH_TWITCH,    /* Do what Twitch does best. */
	TWITCH_RETURN,    /* Return to base. (Low fuel/health.) */
	TWITCH_RECHARGE   /* Seek to middle of base, and wait til fully healed. */
} TwitchMode;

typedef struct TwitchPrivateData {
	Sint8      vx, vy, s;
	Uint8      time_to_change;
	TwitchMode mode;
} TwitchPrivateData;


/* TODO: We need a way to query the level for rock, and this method needs to be
 *       sure that the controller/ai can't see further than a window... */
static void do_exit_up(PublicTankInfo *i, void *d, Sint8 *vx, Sint8 *vy, Uint8 *s) {
	TwitchPrivateData *data = d;
	
	*vx = *vy = 0;
	
	if(i->y < -OUTSIDE) { /* Some point outside the base. */
		data->time_to_change = 0;
		data->mode = TWITCH_TWITCH;
		return;
	}
	
	*vy = -1;
}

static void do_exit_down(PublicTankInfo *i, void *d, Sint8 *vx, Sint8 *vy, Uint8 *s) {
	TwitchPrivateData *data = d;
	
	*vx = *vy = *s = 0;
	
	if(i->y > OUTSIDE) {
		data->time_to_change = 0;
		data->mode = TWITCH_TWITCH;
		return;
	}
	
	*vy = 1;	
}

static void do_twitch(PublicTankInfo *i, void *d, Sint8 *vx, Sint8 *vy, Uint8 *s) {
	TwitchPrivateData *data = d;
	
	if(i->health < TANK_STARTING_SHIELD/2 || i->energy < TANK_STARTING_FUEL/3 ||
	  (abs(i->x) < BASE_SIZE/2 && abs(i->y) < BASE_SIZE/2) ) {
		/* We need a quick pick-me-up... */
		data->mode = TWITCH_RETURN;
	}
	
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

/* Make a simple effort to get back to your base: */
static void do_return(PublicTankInfo *i, void *d, Sint8 *vx, Sint8 *vy, Uint8 *s) {
	TwitchPrivateData *data = d;
	
	/* Seek to the closest entrance: */
	int targety = (i->y < 0) ? -OUTSIDE : OUTSIDE;
	
	/* Check to see if we've gotten there: */
	if((i->x == 0 && i->y == targety) || 
	   (abs(i->x)<BASE_SIZE/2 && abs(i->y)<BASE_SIZE/2)) {
		*s = *vx = *vy = 0;
		data->mode = TWITCH_RECHARGE;
		return;
	}
	
	/* If we are close to the base, we need to navigate around the walls: */
	if( abs(i->x) <= OUTSIDE && abs(i->y) < OUTSIDE ) {
		*s = *vx = 0;
		*vy = (i->y < targety) * 2 - 1;
		return;
	}
	
	/* Else, we will very simply seek to the correct point: */
	*s = 0;
	*vx = i->x != 0       ? ((i->x < 0) * 2 - 1) : 0;
	*vy = i->y != targety ? ((i->y < targety) * 2 - 1) : 0;
}

static void do_recharge(PublicTankInfo *i, void *d, Sint8 *vx, Sint8 *vy, Uint8 *s) {
	TwitchPrivateData *data = d;
	
	/* Check to see if we're fully charged/healed: */
	if(i->health == TANK_STARTING_SHIELD && i->energy == TANK_STARTING_FUEL) {
		data->mode = rand_bool(500) ? TWITCH_EXIT_UP : TWITCH_EXIT_DOWN;
		return;
	}
	
	/* Else, seek to the base's origin, and wait: */
	*s = 0;
	*vx = i->x ? ((i->x < 0) * 2 - 1) : 0;
	*vy = i->y ? ((i->y < 0) * 2 - 1) : 0;
}

static void twitch_controller(PublicTankInfo *i, void *d, Sint8 *vx, Sint8 *vy, Uint8 *s) {
	TwitchPrivateData *data = d;
	
	switch(data->mode) {
		case TWITCH_EXIT_UP:   do_exit_up  (i, d, vx, vy, s); return;
		case TWITCH_EXIT_DOWN: do_exit_down(i, d, vx, vy, s); return;
		case TWITCH_TWITCH:    do_twitch   (i, d, vx, vy, s); return;
		case TWITCH_RETURN:    do_return   (i, d, vx, vy, s); return;
		case TWITCH_RECHARGE:  do_recharge (i, d, vx, vy, s); return;
	}
}

void controller_twitch_attach( Tank *t ) {
	TwitchPrivateData *data = get_object(TwitchPrivateData);
	data->mode = rand_bool(500) ? TWITCH_EXIT_UP : TWITCH_EXIT_DOWN;
	tank_set_controller(t, twitch_controller, data);
}

