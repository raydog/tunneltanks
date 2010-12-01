#!/bin/sh


# The files that we'll kill whenever we find them:
FILE_LIST="
	CMakeFiles
	CMakeCache.txt
	cmake_install.cmake
	Makefile
	install_manifest.txt
	CPackConfig.cmake
	CPackSourceConfig.cmake
	_CPack_Packages
	tunneltanks-*.deb
	tunneltanks-*.tar.bz2
	debug_start.bmp
	debug_end.bmp"


# Check to see if we're root:
if [ `whoami` != "root" ] ; then
	echo "Sorry, this script needs to be run as root."
	echo ""
	echo "I know, I know, it doesn't make sense, but CMake dumps"
	echo "a root-owned file (install_manifest.txt) into this directory, and"
	echo "this script needs permission to remove it."
	exit 1
fi


# Run a make clean, if a Makefile exists:
if [ -f Makefile ] ; then
	make clean
fi

# For each listed directory...
for I in . src ; do
	cd $I
	
	# Remove any file that we don't like:
	rm -rf $FILE_LIST
	
	cd -
done

