#!/bin/sh

# Run a make clean, if a Makefile exists:
if [ -f Makefile ] ; then
	make clean
fi

# Save our current position:
SAVE=`pwd`

# For each listed directory...
for I in . src ; do
	cd $I
	# Remove the CMake files:
	rm -rf CMakeFiles CMakeCache.txt cmake_install.cmake Makefile
	
	# And remove those debug files:
	rm -f debug_start.bmp debug_end.bmp

	cd $SAVE
done

