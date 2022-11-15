cmake_minimum_required(VERSION 3.19)

FetchContent_Declare(
  ZLIB
  URL       https://www.zlib.net/zlib-1.2.13.tar.gz
  URL_HASH  SHA256=b3a24de97a8fdbc835b9833169501030b8977031bcb54b3b3ac13740f846ab30
)
FetchContent_MakeAvailable(ZLIB)
message(STATUS "Fetched ZLIB to ${zlib_SOURCE_DIR}")
set_subdirectory_folder("3rdparty/ZLIB" ${zlib_SOURCE_DIR})
set(ZLIB_FOUND 1)
set(ZLIB_LIBRARY zlibstatic)
set(ZLIB_LIBRARIES zlibstatic)
set(ZLIB_INCLUDE_DIR "${zlib_SOURCE_DIR}" "${zlib_BINARY_DIR}")

set(TABGRAPH_FBX_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/FBX/FBXDocument.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/FBX/FBXNode.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/FBX/FBXObject.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/FBX/FBXProperty.hpp)

set(TABGRAPH_FBX_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/FBX.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/FBXDocument.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/FBXNode.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/FBXObject.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/FBXProperty.cpp)

target_sources(TabGraph-Assets PRIVATE ${TABGRAPH_FBX_HEADER_FILES} ${TABGRAPH_FBX_SOURCE_FILES})
target_include_directories(TabGraph-Assets PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${ZLIB_INCLUDE_DIR}")
target_link_libraries(TabGraph-Assets zlib)
