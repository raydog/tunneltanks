cmake_minimum_required(VERSION 2.6)

################################################################################
### COMPILING THE PROJECT: #####################################################
################################################################################

project(tunneltanks C)

# Verify that SDL is installed:
find_package( SDL REQUIRED )
include_directories( ${SDL_INCLUDE_DIR} )

# Set up the compiler, and find all source files:
set(CMAKE_C_FLAGS "-g -ansi -Wall -O3 -DUSE_SDL_GAMELIB")
file(GLOB source_files "src/*.c" "src/gamelib/SDL/*.c")

# Link and compile:
link_libraries( ${SDL_LIBRARY} )
add_executable( tunneltanks ${source_files} )

# Enable the make install:
install(
	TARGETS tunneltanks
	RUNTIME DESTINATION bin
)


################################################################################
### PACKAGING THE PROJECT: #####################################################
################################################################################

# Set all the files needed for proper packaging:
CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/pack/config.cpack.in      ${PROJECT_SOURCE_DIR}/pack/config.cpack      @ONLY)
CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/pack/target.tbz2.cpack.in ${PROJECT_SOURCE_DIR}/pack/target.tbz2.cpack @ONLY)
CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/pack/target.deb.cpack.in  ${PROJECT_SOURCE_DIR}/pack/target.deb.cpack  @ONLY)
CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/pack/target.rpm.cpack.in  ${PROJECT_SOURCE_DIR}/pack/target.rpm.cpack  @ONLY)

# Add a custom target to make a binary tarball:
add_custom_target(package-tbz2
	${CMAKE_CPACK_COMMAND}
	--config ${CMAKE_SOURCE_DIR}/pack/target.tbz2.cpack
)

# Another custom target to make a deb file: (For Debian/Ubuntu/Mint/etc...)
add_custom_target(package-deb
	${CMAKE_CPACK_COMMAND}
	--config ${CMAKE_SOURCE_DIR}/pack/target.deb.cpack
)

# Another custom target to make a rpm file: (For Fedora/Red Hat/etc...)
add_custom_target(package-rpm
	${CMAKE_CPACK_COMMAND}
	--config ${CMAKE_SOURCE_DIR}/pack/target.rpm.cpack
)


################################################################################
### CLEANING THE PROJECT: ######################################################
################################################################################

# One other final custom target for a dist-clean:
add_custom_target(dist-clean
	rm -rf
	CMakeFiles
	CMakeCache.txt
	cmake_install.cmake
	Makefile
	install_manifest.txt
	CPackConfig.cmake
	CPackSourceConfig.cmake
	_CPack_Packages
	tunneltanks
	tunneltanks_*.deb
	tunneltanks_*.tar.bz2
	tunneltanks_*.rpm
	debug_start.bmp
	debug_end.bmp
	pack/*.cpack
)

