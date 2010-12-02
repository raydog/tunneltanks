#include "level.h"
#include "levelgentunneler.h"
#include "random.h"

#include "level_defn.h"

#define BORDER    30

static Vector pt_rand(unsigned w, unsigned h) {
	Vector out;
	out.x = rand_int(BORDER, w - BORDER);
	out.y = rand_int(BORDER, h - BORDER);
	return out;
}

static unsigned pt_dist(Vector a, Vector b) {
	return (a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y);
}

static void fill_all(Level *lvl, char c) {
	register unsigned i;
	
	for(i=0; i<lvl->width * lvl->height; i++) {
		lvl->array[i] = c;
	}
}

static void add_spawns(Level *lvl) {
	unsigned i, j;
	
	lvl->spawn[0] = pt_rand(lvl->width, lvl->height);
	
	for(i=1; i<MAX_TANKS; i++) {
		int done = 0;
		while(!done) {
			/* Try adding a new point: */
			lvl->spawn[i] = pt_rand(lvl->width, lvl->height);
			
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

