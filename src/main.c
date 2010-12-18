#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "screen.h"
#include "tweak.h"
#include "memalloc.h"
#include "level.h"
#include "levelgen.h"
#include "random.h"
#include "drawbuffer.h"
#include "projectile.h"
#include "types.h"
#include "controller.h"
#include "tanklist.h"
#include "guisprites.h"
#include "gamelib/gamelib.h"


/* Keeping this here isn't the most elegant thing, but I plan on removing the
 * bitmap thing entirely later, so I don't care: */
static int __DEBUG_DUMP_BITMAPS = 0;


/* TODO: We should break some of the game-related crap into another file, so
 *       that this file doesn't balloon in size when we introduce the menus, or
 *       the overview map... */

static void twitch_fill(TankList *tl, Level *lvl, unsigned starting_id) {
	unsigned i;
	
	for(i=starting_id; i<MAX_TANKS; i++) {
		Tank *t = tanklist_add_tank(tl, i, level_get_spawn(lvl, i));
		controller_twitch_attach(t);
	}
}

void init_single_player(Screen *s, TankList *tl, Level *lvl) {
	Tank *t;
	
	/* Ready the tank! */
	t = tanklist_add_tank(tl, 0, level_get_spawn(lvl, 0));
	gamelib_tank_attach(t, 0, 1);
	
	screen_add_window(s, RECT(2, 2, GAME_WIDTH-4, GAME_HEIGHT-6-STATUS_HEIGHT), t);
	screen_add_status(s, RECT(9, GAME_HEIGHT - 2 - STATUS_HEIGHT, GAME_WIDTH-12, STATUS_HEIGHT), t, 1);
	
	/* Add the GUI bitmaps: */
	screen_add_bitmap(s, RECT(3, GAME_HEIGHT - 2 - STATUS_HEIGHT    , 4, 5), GUI_ENERGY, &color_status_energy);
	screen_add_bitmap(s, RECT(3, GAME_HEIGHT - 2 - STATUS_HEIGHT + 6, 4, 5), GUI_HEALTH, &color_status_health);
	
	/* Fill up the rest of the slots with Twitches: */
	twitch_fill(tl, lvl, 1);
}

void init_double_player(Screen *s, TankList *tl, Level *lvl) {
	Tank *t;
	
	/* Ready the tanks! */
	t = tanklist_add_tank(tl, 0, level_get_spawn(lvl, 0));
	gamelib_tank_attach(t, 0, 2);
	screen_add_window(s, RECT(2, 2, GAME_WIDTH/2-3, GAME_HEIGHT-6-STATUS_HEIGHT), t);
	screen_add_status(s, RECT(3, GAME_HEIGHT - 2 - STATUS_HEIGHT, GAME_WIDTH/2-5-2, STATUS_HEIGHT), t, 0);
	
	/* Load up two controllable tanks: */
	t = tanklist_add_tank(tl, 1, level_get_spawn(lvl, 1));
	
	/*controller_twitch_attach(t);  << Attach a twitch to a camera tank, so we can see if they're getting smarter... */
	gamelib_tank_attach(t, 1, 2);
	screen_add_window(s, RECT(GAME_WIDTH/2+1, 2, GAME_WIDTH/2-3, GAME_HEIGHT-6-STATUS_HEIGHT), t);
	screen_add_status(s, RECT(GAME_WIDTH/2+2+2, GAME_HEIGHT - 2 - STATUS_HEIGHT, GAME_WIDTH/2-5-3, STATUS_HEIGHT), t, 1);

	/* Add the GUI bitmaps: */
	screen_add_bitmap(s, RECT(GAME_WIDTH/2-2, GAME_HEIGHT - 2 - STATUS_HEIGHT    , 4, 5), GUI_ENERGY, &color_status_energy);
	screen_add_bitmap(s, RECT(GAME_WIDTH/2-2, GAME_HEIGHT - 2 - STATUS_HEIGHT + 6, 4, 5), GUI_HEALTH, &color_status_health);
	
	/* Fill up the rest of the slots with Twitches: */
	twitch_fill(tl, lvl, 2);
}


typedef struct DrawingData {
	Level      *lvl;
	TankList   *tl;
	DrawBuffer *b;
	PList      *pl;
	Screen     *s;
} DrawingData;


int update_game(void *d) {
	DrawingData *data = (DrawingData *)d;
	EventType temp;
	
	/* Handle all queued events: */
	while( (temp=gamelib_event_get_type()) != GAME_EVENT_NONE ) {
		
		/* Trying to resize the window? */
		if(temp == GAME_EVENT_RESIZE) {
			Rect r = gamelib_event_resize_get_size();
			screen_resize(data->s, r.w, r.h);
		
		/* Trying to toggle fullscreen? */
		} else if(temp == GAME_EVENT_TOGGLE_FULLSCREEN) {
			screen_set_fullscreen(data->s, -1);
		
		/* Trying to exit? */
		} else if(temp == GAME_EVENT_EXIT) {
			if(__DEBUG_DUMP_BITMAPS)
				level_dump_bmp(data->lvl, "debug_end.bmp");
			
			drawbuffer_destroy(data->b);
			plist_destroy(data->pl);
			tanklist_destroy(data->tl);
			level_destroy(data->lvl);
			return 1;
		
		}
		
		/* Done with this event: */
		gamelib_event_done();
	}
	
	/* Clear everything: */
	tanklist_map(data->tl, tank_clear(t, data->b));
	plist_clear(data->pl, data->b);

	/* Charge a small bit of energy for life: */
	tanklist_map(data->tl, tank_alter_energy(t, TANK_IDLE_COST));

	/* See if we need to be healed: */
	tanklist_map(data->tl, tank_try_base_heal(t));
	
	/* Move everything: */
	plist_step(data->pl, data->lvl, data->tl);
	tanklist_map(data->tl, tank_move(t, data->tl));
	
	/* Draw everything: */
	plist_draw(data->pl, data->b);
	tanklist_map(data->tl, tank_draw(t, data->b));
	screen_draw(data->s);
	
	return 0;
}


/* TODO: We need a configuration structure. These args are getting out-of-hand: */
void play_game(Screen *s, char *id, unsigned width, unsigned height, int player_count) {
	DrawingData data;
	
	/* Initialize most of the structures: */
	data.pl  = plist_new();
	data.b   = drawbuffer_new(width, height);
	data.lvl = level_new(data.b, width, height);
	data.tl  = tanklist_new(data.lvl, data.pl);
	data.s   = s;
	
	/* Generate our random level: */
	generate_level(data.lvl, id);
	level_decorate(data.lvl);
	level_make_bases(data.lvl);
	if(__DEBUG_DUMP_BITMAPS)
		level_dump_bmp(data.lvl, "debug_start.bmp");
	
	/* Start drawing! */
	drawbuffer_set_default(data.b, color_rock);
	level_draw_all(data.lvl, data.b);
	screen_set_mode_level(data.s, data.b);
	
	/* Set up the players/GUI: */
	if(player_count == 1)
		init_single_player(data.s, data.tl, data.lvl);
	else
		init_double_player(data.s, data.tl, data.lvl);
	
	gamelib_main_loop(update_game, &data);
}

	
int main(int argc, char *argv[]) {
	Screen *s;
	unsigned i, is_reading_level=0, is_reading_seed=0, is_reading_file=0;
	unsigned fullscreen=0, width=1000, height=500, player_count = 2;
	char *id = NULL, *outfile_name = NULL;
	int seed = 0, manual_seed=0;
	
	/* Iterate through the CLAs: */
	for(i=1; i<argc; i++) {
		if(is_reading_level) {
			id = argv[i];
			is_reading_level = 0;
		
		} else if(is_reading_seed) {
			seed = atoi(argv[i]);
			manual_seed = 1;
			is_reading_seed = 0;
		
		} else if(is_reading_file) {
			outfile_name = argv[i];
			is_reading_file = 0;
		
		} else if( !strcmp("--help", argv[i]) ) {
			printf("%s %s\n\n", WINDOW_TITLE, VERSION);
			
			printf("--version          Display version, and exit.\n");
			printf("--help             Display this help message and exit.\n\n");
			
			printf("--single           Only have one user-controlled tank.\n");
			printf("--double           Have two user-controlled tanks. (Default)\n\n");
			
			printf("--show-levels      List all available level generators.\n");
			printf("--level <GEN>      Use <GEN> as the level generator.\n");
			printf("--seed <INT>       Use <INT> as the random seed.\n");
			printf("--large            Generate a far larger level.\n");
			printf("--fullscreen       Start in fullscreen mode.\n\n");
			printf("--only-gen <FILE>  Will only write the level to a .bmp file, and exit.\n");
			printf("--debug            Write before/after .bmp's to current directory.\n");
			
			return 0;
		
		} else if( !strcmp("--version", argv[i]) ) {
			printf("%s %s\n", WINDOW_TITLE, VERSION);
			return 0;
		
		
		} else if( !strcmp("--single", argv[i]) ) {
			player_count = 1;
		
		} else if( !strcmp("--double", argv[i]) ) {
			player_count = 2;
		
		
		} else if( !strcmp("--show-levels", argv[i]) ) {
			print_levels(stdout);
			return 0;
		
		} else if( !strcmp("--level", argv[i]) ) {
			is_reading_level = 1;
		
		} else if( !strcmp("--seed", argv[i]) ) {
			is_reading_seed = 1;
		
		} else if( !strcmp("--large", argv[i]) ) {
			width = 1500; height = 750;
		
		} else if( !strcmp("--debug", argv[i]) ) {
			__DEBUG_DUMP_BITMAPS = 1;
		
		} else if( !strcmp("--fullscreen", argv[i]) ) {
			fullscreen = 1;
		
		} else if( !strcmp("--only-gen", argv[i]) ) {
			is_reading_file = 1;
		
		} else {
			fprintf(stderr, "Unexpected argument: '%s'\n", argv[i]);
			exit(1);
		}
	}
	
	
	
	/* Seed if necessary: */
	if(manual_seed) srand(seed);
	else            rand_seed();
	
	/* If we're only writing the generated level to file, then just do that: */
	if(outfile_name) {
		Level *lvl = level_new(NULL, width, height);
	
		/* Generate our random level: */
		generate_level(lvl, id);
		level_decorate(lvl);
		level_make_bases(lvl);
		
		/* Dump it out, and exit: */
		level_dump_bmp(lvl, outfile_name);

		gamelib_exit();
		return 0;
	}
	
	/* Let's get this ball rolling: */
	gamelib_init();
	s = screen_new(fullscreen);
	
	/* Play the game: */
	play_game(s, id, width, height, player_count);
	
	/* Ok, we're done. Tear everything up: */
	screen_destroy(s);
	gamelib_exit();
	print_mem_stats();
	
	return 0;
}
