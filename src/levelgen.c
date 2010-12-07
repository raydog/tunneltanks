#include <stdio.h>
#include <string.h>
#include <time.h>
#include "levelgen.h"


typedef struct LevelGenerator {
	char              *id;
	LevelGeneratorFunc gen;
	char              *desc;
} LevelGenerator;
#define LEVEL_GENERATOR(id, gen, desc) ((LevelGenerator){(id), (gen), (desc)})


/* === All the generator headers go here: =================================== */

#include "levelgentoast.h"
#include "levelgensimple.h"
#include "levelgenmaze.h"

/* Add an entry for every generator: */
LevelGenerator GENERATOR_LIST[] =
{
	LEVEL_GENERATOR("toast",  toast_generator,  "Twisty, cavernous maps." ),
	LEVEL_GENERATOR("simple", simple_generator, "Simple rectangular maps with ragged sides."),
	LEVEL_GENERATOR("maze",   maze_generator,   "Complicated maps with a maze surrounding the bases."),
	
	/* This needs to be the last item in the list: */
	LEVEL_GENERATOR(NULL, NULL, NULL)
};

/* ========================================================================== */

#define TIMER_START(t) \
	(t) = clock()

#define TIMER_STOP(t) do { \
	unsigned temp = ((clock() - (t)) * 100) / CLOCKS_PER_SEC; \
	printf("%u.%02u sec\n", temp/100, temp%100); \
} while(0)

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

/* Will print a specified number of spaces to the file: */
static void put_chars(FILE *f, int i, char c) {
	while( i --> 0 )
		fprintf(f, "%c", c);
}

void print_levels(FILE *out) {
	unsigned i, max_id = 7, max_desc = strlen("Description:");
	
	/* Get the longest ID/Description length: */
	for(i=0; GENERATOR_LIST[i].id; i++) {
		if(strlen(GENERATOR_LIST[i].id) > max_id)
			max_id = strlen(GENERATOR_LIST[i].id);
		if(strlen(GENERATOR_LIST[i].desc) > max_desc)
			max_desc = strlen(GENERATOR_LIST[i].desc);
	}
	
	/* Print the header: */
	fprintf(out, "ID:  ");
	put_chars(out, max_id - strlen("ID:"), ' ');
	fprintf(out, "Description:\n");
	put_chars(out, max_id + max_desc + 2, '-');
	fprintf(out, "\n");
	
	/* Print all things: */
	for(i=0; GENERATOR_LIST[i].id; i++) {
		fprintf(out, "%s  ", GENERATOR_LIST[i].id);
		put_chars(out, max_id - strlen(GENERATOR_LIST[i].id), ' ');
		fprintf(out, "%s%s\n", GENERATOR_LIST[i].desc, i==0 ? " (Default)":"");
	}
	fprintf(out, "\n");
}

