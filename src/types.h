#ifndef _TYPES_H_
#define _TYPES_H_

/* Generic types that are used all over the place: */

/* A very simple struct used to store spawn locations of tanks: */
typedef struct Vector {
	unsigned x, y;
} Vector;
#define VECTOR(x,y) ((Vector){(x),(y)})

#endif /* _TYPES_H_ */

