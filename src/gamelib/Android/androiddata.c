#include <stdlib.h>
#include <types.h>
#include "androiddata.h"

#include "require_android.h"

AndroidData _DATA = {
	.prev          = RECT(0,0,0,0),
	.c_touch       = VECTOR(0,0),
	.c_dir         = VECTOR(0,0),
	.c_button      = 0,
	.c_is_touching = 0,
	.env           = NULL,
	.bitmap        = NULL,
	.gd            = NULL
};

