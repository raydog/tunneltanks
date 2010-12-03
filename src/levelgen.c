#include <stdio.h>
#include <string.h>
#include <time.h>
#include "levelgen.h"


typedef struct LevelGenerator {
	char              *id;
	LevelGeneratorFunc gen;
} LevelGenerator;
#define LEVEL_GENERATOR(id, gen) ((LevelGenerator){(id), (gen)})


/* === All the generator headers go here: =================================== */

#include "levelgentoast.h"
#include "levelgentunneler.h"

/* Add an entry for every generator: */
LevelGenerator GENERATOR_LIST[] =
{
	LEVEL_GENERATOR("toast",    toast_generator),
	LEVEL_GENERATOR("tunneler", tunneler_generator),
	
	/* This needs to be the last item in the list: */
	LEVEL_GENERATOR(NULL, NULL)
};

/* ========================================================================== */


/* TODO: Stop using floats in this: */

#define TIMER_START(t) \
	(t) = clock()

#define TIMER_STOP(t) \
	printf("%.2lf sec\n", ((double)(clock()-(t)))/CLOCKS_PER_SEC)


/* Linear search is ok here, cause this function should be rarely called, and
 * there aren't many level generators: */

void generate_level(Level *lvl, char *id) {
	LevelGeneratorFunc func = NULL;
	unsigned i;
	clock_t t;
	
	/* If 'id' is null, go with the default: */
	if(!id) id = GENERATOR_LIST[0].id;
	
	/* Look for the id: */
	for(i=0; GENERATOR_LIST[i].id; i++) {
		if(!strcmp(id, GENERATOR_LIST[i].id)) {
			printf("Using level generator: '%s'\n", GENERATOR_LIST[i].id);
			func = GENERATOR_LIST[i].gen;
			goto generate_level;
		}
	}
	
	/* Report what level generator we found: */
	printf("Couldn't find level generator: '%s'\n", id);
	printf("Using default level generator: '%s'\n", GENERATOR_LIST[0].id);
	
	/* If we didn't find the id, then we select the default: */
	if(!func) func = GENERATOR_LIST[0].gen;
	
generate_level:

	TIMER_START(t);
	
	/* Ok, now generate the level: */
	func(lvl);
	
	printf("Level generated in: ");
	TIMER_STOP(t);
}

