cmake_minimum_required(VERSION 2.6)

# Needed for the File IO
SET(cat_prog cat)

find_package( SDL REQUIRED )
include_directories( ${SDL_INCLUDE_DIR} )

set(CMAKE_C_FLAGS "-g -ansi -Wall -O3")
file(GLOB source_files "*.c")

link_libraries( ${SDL_LIBRARY} )
add_executable( tunneltanks ${source_files} )

# Todo: don't offer a install option anymore. It dumps root-owned files into the
# project, and makes it so that dist-clean.sh needs root priveleges, which just
# shouldn't happen. Instead, people should generate an rpm, apt, or something
# similar, and install from that.
install(
	TARGETS tunneltanks
	RUNTIME DESTINATION bin
)

# Set all the variables for proper packaging:
include(InstallRequiredSystemLibraries)

# We will be packaging tar.bz2's, deb's, and rpm's:
set(CPACK_GENERATOR "TBZ2" "DEB")
# TODO: Implement a proper "RPM" target

# General settings:
set(CPACK_PACKAGE_VENDOR "Raymond Myers <ray@poweredbytoast.com>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A Tunneler re-implementation, written with SDL. hunh")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "4")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_STRIP_FILES true)


# Stuff needed by the deb packager: (The version numbrs are guessed...)
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.3), libsdl1.2debian (>= 1.2.7)")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER ${CPACK_PACKAGE_VENDOR})
set(CPACK_DEBIAN_PACKAGE_SECTION "games")
include("${PROJECT_SOURCE_DIR}/pack/project_desc")

# Stuff needed by the rpm packager:
# I'll get on this once I have a fedora OS in a VM...

include(CPack)

