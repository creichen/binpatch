project(binpatch)
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

if (CMAKE_BUILD_TYPE STREQUAL "")
  MESSAGE(FATAL_ERROR "You must select a build type from [Debug Release ${CMAKE_CONFIGURATION_TYPES}] via 'cmake -DCMAKE_BUILD_TYPE=<build-type> .'")
endif (CMAKE_BUILD_TYPE STREQUAL "")

set(PACKAGE_NAME "binpatch")
set(PACKAGE_VERSION_MAJOR 0)
set(PACKAGE_VERSION_MINOR 1)
set(PACKAGE_VERSION_PATCH 0)
set(PACKAGE_VERSION_STRING ${PACKAGE_VERSION_MAJOR}.${PACKAGE_VERSION_MINOR}.${PACKAGE_VERSION_PATCH})
set(PACKAGE_VERSION ${PACKAGE_VERSION_STRING})
set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")

add_definitions( -DPACKAGE_VERSION=\"${PACKAGE_VERSION}\" )

# Custom cmake modules
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

#========================================
# gcc

IF(CMAKE_CROSSCOMPILING)
  set(CC_EXTRA_CHECKS "")
ELSE(CMAKE_CROSSCOMPILING)
  set(CC_EXTRA_CHECKS "-Wstrict-aliasing -Wsuggest-attribute=const")
ENDIF(CMAKE_CROSSCOMPILING)

IF("${CFLAGS}" STREQUAL "")
  set(CFLAGS "-Os")
ENDIF("${CFLAGS}" STREQUAL "")

set(CC_BASE_FLAGS "-Wall -Wextra -D_POSIX_C_SOURCE=200809")

set(CMAKE_C_FLAGS_DEBUG "${CFLAGS} -g -std=c99 -pedantic ${CC_BASE_FLAGS} ${CC_EXTRA_FLAGS}")
set(CMAKE_C_FLAGS_RELEASE "${CFLAGS} -std=c99 -pedantic ${CC_BASE_FLAGS} ${CC_EXTRA_FLAGS} -DNDEBUG")

# # ========================================
# # Doxygen
# option(BUILD_DOC "Build documentation" ON) # adapted from https://vicrucann.github.io/tutorials/quick-cmake-doxygen/

# # check if Doxygen is installed
# find_package(Doxygen)
# if (DOXYGEN_FOUND)
#     # set input and output files
#     set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
#     set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

#     # request to configure the file
#     configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
#     message("Doxygen build started")

#     # note the option ALL which allows to build the docs together with the application
#     add_custom_target( doc_doxygen ALL
#         COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
#         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#         COMMENT "Generating API documentation with Doxygen"
#         VERBATIM )
# else (DOXYGEN_FOUND)
#   message("Doxygen must to be installed to generate the doxygen documentation")
# endif (DOXYGEN_FOUND)

#========================================
# valgrind

find_program( MEMORYCHECK_COMMAND valgrind )
set( MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --leak-check=full" )

#========================================
# tests
IF(NOT CMAKE_CROSSCOMPILING)
  enable_testing()
ENDIF(NOT CMAKE_CROSSCOMPILING)

#========================================

include_directories(src)
add_subdirectory(src)

