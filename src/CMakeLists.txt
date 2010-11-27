cmake_minimum_required(VERSION 2.6)

find_package( SDL REQUIRED )
include_directories( ${SDL_INCLUDE_DIR} )

set(CMAKE_C_FLAGS "-g -ansi -Wall -O3")
file(GLOB source_files "*.c")

link_libraries( ${SDL_LIBRARY} )
add_executable( tunneltanks ${source_files} )

#install(TARGETS TunnelTanks 

#include(CPack)
