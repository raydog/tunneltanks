#include "level.h"
#include "levelgentunneler.h"
#include "levelgenutil.h"
#include "random.h"

#include "level_defn.h"

#define BORDER    80

#define SIDE_STEP_MIN  3
#define SIDE_SPEP_MAX  6

#define UP_STEP_MIN    6
#define UP_STEP_MAX    12

/*
static void add_rock(Level *lvl) {

}
*/
static void add_spawns(Level *lvl) {
	unsigned i, j;
	
	lvl->spawn[0] = pt_rand(lvl->width, lvl->height, BORDER);
	
	for(i=1; i<MAX_TANKS; i++) {
		int done = 0;
		while(!done) {
			/* Try adding a new point: */
			lvl->spawn[i] = pt_rand(lvl->width, lvl->height, BORDER);
			
			/* Make sure that point isn't too close to others: */
			for(j=0; j<i; j++) {
				if(pt_dist(lvl->spawn[i],lvl->spawn[j]) < MIN_SPAWN_DIST*MIN_SPAWN_DIST)
					break;
			}
			
			/* We're done if we were able to get through that list: */
			done = (j == i);
		}
	}
}

void tunneler_generator(Level *lvl) {
	fill_all(lvl, 0);
	add_spawns(lvl);
}

