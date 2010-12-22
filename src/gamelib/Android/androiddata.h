#ifndef _ANDROID_DATA_H_
#define _ANDROID_DATA_H_

#include <game.h>
#include <types.h>

typedef struct AndroidData {
	/* The last size of the Bitmap, so we know if it's changed: */
	Rect prev;
	
	/* Three controller variables: */
	int  c_x, c_y, c_shoot;
	
	/* The game context variable: */
	GameData *gd;
} AndroidData;

extern AndroidData _DATA;

#endif /* _ANDROID_DATA_H_ */

