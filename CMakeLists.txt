cmake_minimum_required(VERSION 3.19)
cmake_policy(SET CMP0074 NEW)

LIST(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR}/external)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/GroupSources.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/AddSubdirectory.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/AddRuntimeFiles.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Fetch_Dependencies.cmake)

# ################################################################################
# OPTIONS
# ################################################################################
# if this is ON the user is free to specify custom CXX flags
option(MSG_FORCE_COMPILE_FLAGS "Force MSG compile flags from parent project ?" OFF)
# if this is ON address sanitizer will be added to Debug
option(MSG_ASAN "Enable address sanitizer ?" ON)
option(MSG_ENABLE_QT "Compile Qt binding ?" ON)
# ################################################################################
# OPTIONS
# ################################################################################

project(MSG LANGUAGES CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_DEBUG_TARGET_PROPERTIES INCLUDE_DIRECTORIES)
set(CMAKE_DEBUG_TARGET_PROPERTIES INTERFACE_INCLUDE_DIRECTORIES)
# PREFER STATIC LIBRARIES
# cmake respects the order of extensions when looking for libraries
set(CMAKE_FIND_LIBRARY_SUFFIXES .lib .a ${CMAKE_FIND_LIBRARY_SUFFIXES})

# ################################################################################
# SETUP COMPILE FLAGS
# ################################################################################
### SET LOCAL VARIABLES
set(MSG_CXX_FLAGS_RELEASE_L "")
set(MSG_CXX_FLAGS_RELWITHDEBINFO_L "")
set(MSG_CXX_FLAGS_DEBUG_L "")
### SET LOCAL VARIABLES

### FIGURE OUT WHAT COMPILER WE'RE USING
message(Compiler ID : ${CMAKE_CXX_COMPILER_ID})
message(Compiler variant : ${CMAKE_CXX_COMPILER_FRONTEND_VARIANT})
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  set(MSG_CXX_FLAGS_RELEASE_L "-Ox /Zc:__cplusplus /MT")
  set(MSG_CXX_FLAGS_RELWITHDEBINFO_L "-Ox /Zi /Zc:__cplusplus /MT")
  set(MSG_CXX_FLAGS_DEBUG_L "-Od /Zi /Zc:__cplusplus /MTd")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_COMPILER_FRONTEND_VARIANT MATCHES "MSVC")
  set(MSG_CXX_FLAGS_RELEASE_L "-Ox /MT -Wno-undefined-var-template")
  set(MSG_CXX_FLAGS_RELWITHDEBINFO_L "-Ox /Zi /Zc:__cplusplus /MT -Wno-undefined-var-template")
  set(MSG_CXX_FLAGS_DEBUG_L "-Od /MTd -Wno-undefined-var-template")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_COMPILER_FRONTEND_VARIANT MATCHES "GNU")
  set(CLANG_GNU_OPTIONS "-fpermissive -Wno-undefined-var-template")
  set(MSG_CXX_FLAGS_RELEASE_L "${CLANG_GNU_OPTIONS} -O3 -ffast-math -fno-finite-math-only  ")
  set(MSG_CXX_FLAGS_RELWITHDEBINFO_L "${CLANG_GNU_OPTIONS} ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
  set(MSG_CXX_FLAGS_DEBUG_L "${CLANG_GNU_OPTIONS} -Og -fstack-protector-all")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND CMAKE_CXX_COMPILER_FRONTEND_VARIANT MATCHES "GNU")
  set(CLANG_GNU_OPTIONS "-fpermissive -Wno-undefined-var-template")
  set(MSG_CXX_FLAGS_RELEASE_L "${CLANG_GNU_OPTIONS} -O3 -ffast-math -fno-finite-math-only  ")
  set(MSG_CXX_FLAGS_RELWITHDEBINFO_L "${CLANG_GNU_OPTIONS}")
  set(MSG_CXX_FLAGS_DEBUG_L "${CLANG_GNU_OPTIONS} -Og -fstack-protector-all")
else(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
#We're either using GCC or Clang
  set(MSG_CXX_FLAGS_RELEASE_L "-fpermissive -Ofast -fno-finite-math-only")
  set(MSG_CXX_FLAGS_RELWITHDEBINFO_L "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
  set(MSG_CXX_FLAGS_DEBUG_L "-fpermissive -Og -fstack-protector-all")
endif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
### FIGURE OUT WHAT COMPILER WE'RE USING

### SHOULD WE ADD ADDRESS SANITIZER ?
if (${MSG_ASAN})
  if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(MSG_CXX_FLAGS_DEBUG_L "${MSG_CXX_FLAGS_DEBUG_L} /fsanitize=address")
  else(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    #We're either using GCC or Clang
    set(MSG_CXX_FLAGS_DEBUG_L "${MSG_CXX_FLAGS_DEBUG_L} -fsanitize=address,leak")
  endif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
endif (${MSG_ASAN})
### SHOULD WE ADD ADDRESS SANITIZER ?

### RESET CACHE IN CASE MSG_ASAN CHANGED
if (NOT ${MSG_FORCE_COMPILE_FLAGS})
  unset(MSG_CXX_FLAGS_RELEASE CACHE)
  unset(MSG_CXX_FLAGS_RELWITHDEBINFO CACHE)
  unset(MSG_CXX_FLAGS_DEBUG CACHE)
  set(MSG_CXX_FLAGS_RELEASE "${MSG_CXX_FLAGS_RELEASE_L}" CACHE STRING "Compile flags for Release")
  set(MSG_CXX_FLAGS_RELWITHDEBINFO "${MSG_CXX_FLAGS_RELWITHDEBINFO_L}" CACHE STRING "Compile flags for RelWithDebInfo")
  set(MSG_CXX_FLAGS_DEBUG "${MSG_CXX_FLAGS_DEBUG_L}" CACHE STRING "Compile flags for Debug")
endif (NOT ${MSG_FORCE_COMPILE_FLAGS})
### RESET CACHE IN CASE MSG_ASAN CHANGED

### SET CMAKE COMPILE FLAGS
set(CMAKE_CXX_FLAGS_RELEASE "${MSG_CXX_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${MSG_CXX_FLAGS_RELWITHDEBINFO}")
set(CMAKE_CXX_FLAGS_DEBUG "${MSG_CXX_FLAGS_DEBUG}")
### SET CMAKE COMPILE FLAGS
# ################################################################################
# SETUP COMPILE FLAGS
# ################################################################################

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
