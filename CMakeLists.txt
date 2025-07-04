cmake_minimum_required(VERSION 3.19)
cmake_policy(SET CMP0074 NEW)

LIST(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR}/external)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/GroupSources.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/AddSubdirectory.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/AddRuntimeFiles.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Fetch_Dependencies.cmake)

project(MSG LANGUAGES CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Uncomment for debug
set(CMAKE_DEBUG_TARGET_PROPERTIES INCLUDE_DIRECTORIES)
set(CMAKE_DEBUG_TARGET_PROPERTIES INTERFACE_INCLUDE_DIRECTORIES)
# PREFER STATIC LIBRARIES ########
# cmake respects the order of extensions when looking for libraries
set(CMAKE_FIND_LIBRARY_SUFFIXES .lib .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
# prefer linking against static libraries
message(Compiler ID : ${CMAKE_CXX_COMPILER_ID})
message(Compiler variant : ${CMAKE_CXX_COMPILER_FRONTEND_VARIANT})
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ox /Zc:__cplusplus /MT")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Od /Zc:__cplusplus /MTd /fsanitize=address")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_COMPILER_FRONTEND_VARIANT MATCHES "MSVC")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ox /MT -Wno-undefined-var-template")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Od /MTd -Wno-undefined-var-template")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_COMPILER_FRONTEND_VARIANT MATCHES "GNU")
  set(CLANG_GNU_OPTIONS "-fpermissive -Wno-undefined-var-template")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CLANG_GNU_OPTIONS} -O3 -ffast-math -fno-finite-math-only  ")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CLANG_GNU_OPTIONS} -Og -fstack-protector-all -fsanitize=address,leak")
else(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
#We're either using GCC or Clang
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fpermissive -Ofast -fno-finite-math-only")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fpermissive -Og -fstack-protector-all -fsanitize=address,leak")
endif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
# ------------------- ########

add_definitions(-DGLM_ENABLE_EXPERIMENTAL)
add_definitions(-DGLEW_STATIC)
add_definitions(-DNOMINMAX)

add_subdirectory(libs)
add_subdirectory(tests)

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
