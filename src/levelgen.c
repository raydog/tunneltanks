#include <string.h>
#include "levelgen.h"


typedef struct LevelGenerator {
	char              *id;
	LevelGeneratorFunc gen;
} LevelGenerator;
#define LEVEL_GENERATOR(id, gen) ((LevelGenerator){(id), (gen)})


/* === All the generator headers go here: =================================== */

#include "levelgentoast.h"

/* Add an entry for every generator: */
LevelGenerator GENERATOR_LIST[] =
{
	LEVEL_GENERATOR("toast", toast_generator),
	
	/* This needs to be the last item in the list: */
	LEVEL_GENERATOR(NULL, NULL)
};

/* ========================================================================== */


/* Linear search is ok here, cause this function is rarely called: */
void generate_level(Level *lvl, char *id) {
	LevelGeneratorFunc func = NULL;
	unsigned i;
	
	/* If 'id' is null, go with the default: */
	if(!id) id = GENERATOR_LIST[0].id;
	
	/* Look for the id: */
	for(i=0; GENERATOR_LIST[i].id; i++) {
		if(!strcmp(id, GENERATOR_LIST[i].id)) {
			func = GENERATOR_LIST[i].gen;
			break;
		}
	}
	
	/* If we didn't find the id, then we select the default: */
	if(!func) func = GENERATOR_LIST[0].gen;
	
	/* Ok, now generate the level: */
	func(lvl);
}

