cmake_minimum_required(VERSION 3.19)
cmake_policy(SET CMP0074 NEW)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/GroupSources.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/AddSubdirectory.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Fetch_Dependencies.cmake)

project(TabGraph LANGUAGES CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 17)

# Uncomment for debug
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_DEBUG_TARGET_PROPERTIES INCLUDE_DIRECTORIES)
set(CMAKE_DEBUG_TARGET_PROPERTIES INTERFACE_INCLUDE_DIRECTORIES)
# PREFER STATIC LIBRARIES ########
# cmake respects the order of extensions when looking for libraries
set(CMAKE_FIND_LIBRARY_SUFFIXES .lib .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
# prefer linking against static libraries
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ox /MT")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
else(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast -fpermissive")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og -fpermissive")
endif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
# ------------------- ########
set(GLEW_USE_STATIC_LIBS ON)

add_definitions(-DGLEW_STATIC)
add_definitions(-DNOMINMAX)

add_subdirectory(Tools)
add_subdirectory(ECS)
add_subdirectory(SceneGraph)
add_subdirectory(Renderer)

add_subdirectory(Assets)

add_subdirectory(UnitTests)

find_package(Doxygen)

if (DOXYGEN_FOUND)
  set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
  set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.out)
  configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
  message("Doxygen build started")
  add_custom_target(Documentation
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )
else (DOXYGEN_FOUND)
  message("Doxygen not found")
endif (DOXYGEN_FOUND)
