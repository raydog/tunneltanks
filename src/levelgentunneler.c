#include <stdio.h>
#include "level.h"
#include "levelgentunneler.h"
#include "levelgenutil.h"
#include "random.h"
#include "types.h"

#include "level_defn.h"


/* TODO: This generator uses floats, so we'll need to optimize that... */

#define BORDER         80

#define MAX_SLOPE      2.0
#define MAX_DIVISIONS  100


/* Inserts a point into an array, sorted by x value. Returns index of pt, or -1
 * on a failed insertion: */
static int insert_point(Vector *buf, unsigned *index, Vector pt) {
	int i;
	
	/* Instert the point: */
	buf[(*index)++] = pt;
	
	/* Shuffle it into place: */
	for(i = *index - 1; i > 0 && buf[i-1].x > buf[i].x; i--) {
		Vector temp = buf[i-1];
		buf[i-1] = buf[i];
		buf[i] = temp;
	}
	
	return i;
}

/* This just adds random points for now: */
static void add_rock(Level *lvl) {
	Vector   buf[MAX_DIVISIONS];
	unsigned i, buf_index = 0;
	
	for(i=0; i<MAX_DIVISIONS-2; i++) {
		Vector t = pt_rand(lvl->width, BORDER, 0);
		insert_point(buf, &buf_index, t);
	}
	
	for(i=0; i<buf_index; i++) {
		printf("(%u, %u)\n", buf[i].x, buf[i].y);
	}
}


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
	add_rock(lvl);
	add_spawns(lvl);
}

