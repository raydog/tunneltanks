#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "levelgen.h"
#include "level.h"
#include "memalloc.h"
#include "random.h"
#include "queue.h"

#include "level_defn.h"

/* Configuration Constants: */
#define BORDER    30
#define FILTER    70
#define ODDS      300
#define FILLRATIO 65
#define TREESIZE  150

static Vector pt_rand(unsigned w, unsigned h) {
	Vector out;
	out.x = rand_int(BORDER, w - BORDER);
	out.y = rand_int(BORDER, h - BORDER);
	return out;
}

/* Actually returns the distance^2, but points should still remain in the same
 * order, and this doesn't require a call to sqrt(): */
static unsigned pt_dist(Vector a, Vector b) {
	return (a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y);
}

typedef struct Pairing {
	unsigned dist, a, b;
} Pairing;

#ifdef _TESTING
static void level_draw_ascii(Level *lvl) {
	unsigned x,y;
	
	for(y=0; y<lvl->height; y++) {
		for(x=0; x<lvl->width; x++)
			printf("%c", lvl->array[ y*lvl->width + x ]?'#':' ');
		printf("\n");
	}
}
#endif /* _TESTING */

/*----------------------------------------------------------------------------*
 * STAGE 1: Generate a random tree                                            *
 *----------------------------------------------------------------------------*/

static void set_circle(char *a, unsigned w, unsigned x, unsigned y) {
	register int tx, ty;
	for(ty=-3; ty<=3; ty++) {
		for(tx=-3; tx<=3; tx++) {
			if((tx==-3 || tx==3) && (ty==-3 || ty==3)) continue;
			a[ (y+ty)*w + x + tx ] = 0;
		}
	}
}

#define SWAP(type,a,b) do { type t = (a); (a)=(b); (b)=t; } while(0)

/* New Bresenham's Algorithm-based function: */

void draw_line(Level *dest, Vector a, Vector b) {
	int swap, dx, dy, error, stepy;
	register unsigned x, y;

	/* Swap x and y values when the graph gets too steep to operate normally: */
	if((swap = abs(b.y - a.y) > abs(b.x - a.x))) {
		SWAP(unsigned, a.x, a.y);
		SWAP(unsigned, b.x, b.y);
	}

	/* Swap a and b so that a is to the left of b: */
	if(a.x > b.x) SWAP(Vector, a, b);

	/* A few calculations: */
	dx = b.x - a.x;
	dy = abs(b.y - a.y);
	error = dx / 2;
	stepy = (a.y < b.y) ? 1 : -1;
	
	/* Now, for every x from a.x to b.x, add the correct dot: */
	for (x = a.x, y=a.y; x <= b.x; x++) {
		if(swap) set_circle(dest->array, dest->width, y, x);
		else     set_circle(dest->array, dest->width, x, y);

		error -= dy;
		if(error < 0) {
			y     += stepy;
			error += dx;
		}
	}
}


/* Original DDA-based function:

#define ROUND(x) ((unsigned)((x)+0.5))
#define SWAP(a,b) do { Vector t = (a); (a)=(b); (b)=t; } while(0)

static void draw_line(Level *dest, Vector a, Vector b) {
	double x, y, dx, dy, stepx, stepy;
	
	dx = (double)a.x - (double)b.x;
	dy = (double)a.y - (double)b.y;
	
	if(dx==0 && dy==0) {
		set_circle(dest->array, dest->width, a.x, a.y);
		return;
	
	} else if(dx==0 || abs(dy)>abs(dx)) {
		if(a.y > b.y) SWAP(a,b);
		stepx = dx / dy; stepy = 1;
	 
	} else {
		if(a.x > b.x) SWAP(a,b);
		stepx = 1; stepy = dy / dx;
	}
	
	x = a.x; y = a.y;
	while ((stepx==1 && x<=b.x) || (stepy==1 && y<=b.y)) {
		set_circle(dest->array, dest->width, ROUND(x), ROUND(y));
		x += stepx; y += stepy;
	}
}
*/

static int pairing_cmp(const void *a, const void *b) {
	return ((Pairing *)a)->dist - ((Pairing *)b)->dist;
}

static void generate_tree(Level *lvl) {
	unsigned *dsets, paircount;
	register unsigned i, j, k;
	Vector *points;
	Pairing *pairs;
	
	/* Get an array of disjoint set IDs: */
	dsets = get_mem( sizeof(unsigned) * TREESIZE );
	for(i=0; i<TREESIZE; i++) dsets[i] = i;
	
	/* Randomly generate all points: */
	points = get_mem( sizeof(Vector) * TREESIZE );
	for(i=0; i<TREESIZE; i++) points[i] = pt_rand(lvl->width, lvl->height);
	
	/* While we're here, copy in some of those points: */
	lvl->spawn[0] = points[0];
	for(i=1,j=1; i<TREESIZE && j<MAX_TANKS; i++) {
		for(k=0; k<j; k++) {
			if(pt_dist(points[i],lvl->spawn[k]) < MIN_SPAWN_DIST*MIN_SPAWN_DIST)
				break;
		}
		
		if(k!=j) continue;
		lvl->spawn[j++] = points[i];
	}
	if(j!=MAX_TANKS) {
		/* TODO: More robust error handling. */
		printf("OH SHIT OH SHIT OH SHIT\n");
		exit(1);
	}
	/* Get an array of all point-point pairings: */
	paircount = TREESIZE*(TREESIZE+1) / 2;
	pairs = get_mem( sizeof(Pairing) * paircount );
	
	/* Set up all the pairs, and sort them: */
	for(k=i=0; i<TREESIZE; i++)
		for(j=i+1; j<TREESIZE; j++, k++) {
			pairs[k].a = i; pairs[k].b = j;
			pairs[k].dist = pt_dist(points[i], points[j]);
		}
	qsort(pairs, paircount, sizeof(Pairing), pairing_cmp);
	
	for(i=j=0; i<paircount; i++) {
		unsigned aset, bset;
		
		/* Trees only have |n|-1 edges, so call it quits if we've selected that
		 * many: */
		if(j>=TREESIZE-1) break;
		
		aset = dsets[pairs[i].a]; bset = dsets[pairs[i].b];
		if(aset == bset) continue;
		
		/* Else, these points are in different disjoint sets. "Join" them by
		 * drawing them, and merging the two sets: */
		j+=1;
		for(k=0; k<TREESIZE; k++) if(dsets[k] == bset) dsets[k] = aset;
		draw_line(lvl, points[pairs[i].a], points[pairs[i].b]);
	}
	
	/* We don't need this data anymore: */
	free_mem(pairs);
	free_mem(points);
	free_mem(dsets);
}


/*----------------------------------------------------------------------------*
 * STAGE 2: Randomly expand upon the tree                                     *
 *----------------------------------------------------------------------------*/

/* Some cast-to-int tricks here may be fun... ;) */
static int has_neighbor(Level *lvl, unsigned x, unsigned y) {
	if(!lvl->array[ (y-1)*lvl->width + (x-1) ]) return 1;
	if(!lvl->array[ (y-1)*lvl->width + (x  ) ]) return 1;
	if(!lvl->array[ (y-1)*lvl->width + (x+1) ]) return 1;
	if(!lvl->array[ (y  )*lvl->width + (x-1) ]) return 1;
	if(!lvl->array[ (y  )*lvl->width + (x+1) ]) return 1;
	if(!lvl->array[ (y+1)*lvl->width + (x-1) ]) return 1;
	if(!lvl->array[ (y+1)*lvl->width + (x  ) ]) return 1;
	if(!lvl->array[ (y+1)*lvl->width + (x+1) ]) return 1;
	return 0;
}

static void set_outside(Level *lvl, char val) {
	register unsigned i;
	unsigned w = lvl->width, h = lvl->height;
	
	for(i=0; i<w; i++)   lvl->array[i]           = val;
	for(i=0; i<w; i++)   lvl->array[(h-1)*w + i] = val;
	for(i=1; i<h-1; i++) lvl->array[i*w]         = val;
	for(i=1; i<h-1; i++) lvl->array[i*w + w - 1] = val;
}

static void expand_init(Level *lvl, Queue *q) {
	register unsigned x, y;
	
	for(y=1; y<lvl->height-1; y++)
		for(x=1; x<lvl->width-1; x++)
			if(lvl->array[y*lvl->width+x] && has_neighbor(lvl, x, y)) {
				lvl->array[y*lvl->width+x] = 2;
				queue_enqueue(q, &(Vector){x,y});
			}
}

#define MIN2(a,b)   ((a<b) ? a : b)
#define MIN3(a,b,c) ((a<b) ? a : (b<c) ? b : c)
static unsigned expand_once(Level *lvl, Queue *q) {
	Vector temp;
	unsigned i, j, total, count = 0;
	
	total = queue_length(q);
	for(i=0; i<total; i++) {
		unsigned xodds, yodds, odds;
		
		temp = queue_dequeue(q);
		
		xodds = ODDS * MIN2(lvl->width - temp.x,  temp.x) / FILTER;
		yodds = ODDS * MIN2(lvl->height - temp.y, temp.y) / FILTER;
		odds  = MIN3(xodds, yodds, ODDS);
		
		if(rand_bool(odds)) {
			lvl->array[ temp.y*lvl->width + temp.x ] = 0;
			count++;
			
			/* Now, queue up any neighbors that qualify: */
			for(j=0; j<9; j++) {
				char *c;
				unsigned tx, ty;
				
				if(j==4) continue;
				
				tx = temp.x + (j%3) - 1; ty = temp.y + (j/3) - 1;
				c = &lvl->array[ty*lvl->width + tx];
				if(*c == 1) {
					*c = 2;
					queue_enqueue(q, &(Vector){tx, ty});
				}
			}
		} else
			queue_enqueue(q, &temp);
	}
	return count;
}

static void expand_cleanup(Level *lvl) {
	register unsigned x, y;
	
	for(y=0; y<lvl->height; y++)
		for(x=0; x<lvl->width; x++)
			lvl->array[y*lvl->width+x] = !! lvl->array[y*lvl->width+x];
}

static void randomly_expand(Level *lvl) {
	unsigned cur = 0, goal = lvl->width * lvl->height * FILLRATIO / 100;
	Queue *q;
	
	/* Experimentally, the queue never grew to larger than 3/50ths of the level
	 * size, so we can use that to save quite a bit of memory: */
	q = queue_new(lvl->width * lvl->height * 3 / 50);
	
	expand_init(lvl, q);
	while( (cur += expand_once(lvl, q)) < goal );
	expand_cleanup(lvl);
	
	queue_destroy(q);
}


/*----------------------------------------------------------------------------*
 * STAGE 3: Smooth out the graph with a cellular automaton                    *
 *----------------------------------------------------------------------------*/

static int count_neighbors(Level *lvl, unsigned x, unsigned y) {
	unsigned w = lvl->width;
	return lvl->array[ (y-1)*w + (x-1) ] +
	       lvl->array[ (y-1)*w + (x  ) ] +
	       lvl->array[ (y-1)*w + (x+1) ] +
	       lvl->array[ (y  )*w + (x-1) ] +
	       lvl->array[ (y  )*w + (x+1) ] +
	       lvl->array[ (y+1)*w + (x-1) ] +
	       lvl->array[ (y+1)*w + (x  ) ] +
	       lvl->array[ (y+1)*w + (x+1) ];
}

#define MIN2(a,b)   ((a<b) ? a : b)
#define MIN3(a,b,c) ((a<b) ? a : (b<c) ? b : c)
static unsigned smooth_once(Level *lvl) {
	register unsigned x, y, count = 0;
	
	for(y=1; y<lvl->height-1; y++)
		for(x=1; x<lvl->width-1; x++) {
			int n;
			char oldbit = lvl->array[ y*lvl->width + x ];
			
			n = count_neighbors(lvl, x, y);
			lvl->array[ y*lvl->width + x ] = oldbit ? (n>=3) : (n>4);
			
			count += lvl->array[ y*lvl->width + x ] != oldbit;
		}
	return count;
}

static void smooth_cavern(Level *lvl) {
	set_outside(lvl, 0);
	while(smooth_once(lvl));
	set_outside(lvl, 1);
}


/*----------------------------------------------------------------------------*
 * MAIN FUNCTIONS:                                                            *
 *----------------------------------------------------------------------------*/

#define TIMER_START(t) \
	(t) = clock()

#define TIMER_STOP(t) \
	printf("%.2lf sec\n", ((double)(clock()-(t)))/CLOCKS_PER_SEC)
	
void toast_generator(Level *lvl) {
	clock_t t;
	
	TIMER_START(t);
	
	generate_tree(lvl);
	randomly_expand(lvl);
	smooth_cavern(lvl);
	
	printf("Level generated in: ");
	TIMER_STOP(t);
}

#ifdef _TESTING

int main() {
	clock_t t, all;
	Level lvl;
	unsigned i;
	
	rand_seed();
	
	/* We don't need a full-fledged Level object, so let's just half-ass one: */
	lvl.width = 1000; lvl.height = 500;
	lvl.array = get_mem(sizeof(char) * lvl.width * lvl.height);
	for(i=0; i<lvl.width * lvl.height; i++) lvl.array[i] = 1;
	
	TIMER_START(all);
	TIMER_START(t);
	generate_tree(&lvl);
	TIMER_STOP(t);
	
	TIMER_START(t);
	randomly_expand(&lvl);
	TIMER_STOP(t);
	
	TIMER_START(t);
	smooth_cavern(&lvl);
	TIMER_STOP(t);
	
	printf("-----------\nTotal: ");
	TIMER_STOP(all);
	
	level_draw_ascii(&lvl);
	
	free_mem(lvl.array);
	
	print_mem_stats();
	return 0;
}
#endif /* _TESTING */

