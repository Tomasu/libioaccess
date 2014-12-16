cmake_minimum_required(VERSION 2.8)
project(libioaccess CXX)

set(LIBIOACCESS_MAJOR_VERSION 0)
set(LIBIOACCESS_MINOR_VERSION 1)
set(LIBIOACCESS_PATCH_VERSION 0)
set(LIBIOACCESS_VERSION ${LIBIOACCESS_MAJOR_VERSION}.${LIBIOACCESS_MINOR_VERSION}.${LIBIOACCESS_PATCH_VERSION})

if(CMAKE_BUILD_TYPE STREQUAL Debug)
	add_definitions(-DDEBUG -O0 -ggdb3)
else()
	add_definitions(-O2 -s -march=native)
endif()

add_definitions(-fPIC -Wextra -Wall -pipe -std=c++11)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include CACHE PATH "Installation directory for header files")

if(WIN32 AND NOT CYGWIN)
	set(DEF_INSTALL_CMAKE_DIR CMake)
else()
	set(DEF_INSTALL_CMAKE_DIR lib/cmake/LibMCMap)
endif()
	
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH "Installation directory for CMake files")
		
# Make relative paths absolute (needed later on)
foreach(p LIB BIN INCLUDE CMAKE)
	set(var INSTALL_${p}_DIR)
	if(NOT IS_ABSOLUTE "${${var}}")
		set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
	endif()
endforeach()

set(IOACCESS_PUBLIC_HEADERS
	include/ioaccess/IOAccess.h
)

set(IOACCESS_HEADERS
	${IOACCESS_PUBLIC_HEADERS}
	include/ioaccess/StdioInterface.h
	include/ioaccess/StdioInterfacePrivate.h
)

set(IOACCESS_SOURCES
	src/IOAccess.cpp
	src/StdioInterface.cpp
)

add_library(ioaccess SHARED ${IOACCESS_SOURCES} ${IOACCESS_HEADERS})

set_target_properties(ioaccess PROPERTIES PUBLIC_HEADER "${IOACCESS_PUBLIC_HEADERS}")

# Add all targets to the build-tree export set
export(TARGETS ioaccess
FILE "${PROJECT_BINARY_DIR}/LibIOAccessTargets.cmake")

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE LibIOAccess)

# Create the LibNBTConfig.cmake and LibNBTConfigVersion files
file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}"
"${INSTALL_INCLUDE_DIR}")

# ... for the build tree
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include" "${PROJECT_BINARY_DIR}")
configure_file(cmake/LibIOAccessConfig.cmake.in
"${PROJECT_BINARY_DIR}/LibIOAccessConfig.cmake" @ONLY)

# ... for the install tree
set(CONF_INCLUDE_DIRS "\${LIBIOACCESS_CMAKE_DIR}/${REL_INCLUDE_DIR}/libioaccess")
configure_file(cmake/LibIOAccessConfig.cmake.in
"${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/LibIOAccessConfig.cmake" @ONLY)
# ... for both
configure_file(cmake/LibIOAccessConfigVersion.cmake.in
"${PROJECT_BINARY_DIR}/LibIOAccessConfigVersion.cmake" @ONLY)

# Install the LibMCMapConfig.cmake and LibMCMapConfigVersion.cmake
install(FILES
"${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/LibIOAccessConfig.cmake"
"${PROJECT_BINARY_DIR}/LibIOAccessConfigVersion.cmake"
DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)

# Install the export set for use with the install-tree
install(EXPORT LibIOAccessTargets DESTINATION
"${INSTALL_CMAKE_DIR}" COMPONENT dev)

install(TARGETS ioaccess
# IMPORTANT: Add the libioaccess library to the "export-set"
EXPORT LibIOAccessTargets
RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/libioaccess"
COMPONENT dev)

