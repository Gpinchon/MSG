if (NOT ZLIB_FOUND)
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
endif()

if (NOT PNG_FOUND)
  FetchContent_Declare(
      PNG
      URL     http://prdownloads.sourceforge.net/libpng/libpng-1.6.38.tar.gz?download
      URL_HASH  SHA256=e2b5e1b4329650992c041996cf1269681b341191dc07ffed816c555769cceb77
  )
  # Public CMake configuration variables.
  option(PNG_SHARED "Build shared lib" OFF)
  option(PNG_STATIC "Build static lib" ON)
  option(PNG_EXECUTABLES "Build libpng executables" OFF)
  option(PNG_TESTS "Build libpng tests" OFF)
  # Many more configuration options could be added here.
  option(PNG_FRAMEWORK "Build OS X framework" OFF)
  option(PNG_DEBUG "Build with debug output" OFF)
  option(PNG_HARDWARE_OPTIMIZATIONS "Enable hardware optimizations" ON)
  option(SKIP_INSTALL_ALL "Skip install" ON)
  FetchContent_MakeAvailable(PNG)
  set_subdirectory_folder("3rdparty/PNG" ${png_SOURCE_DIR})
  set(PNG_FOUND 1)
  set(PNG_LIBRARY png_static)
  set(PNG_LIBRARIES png_static)
  set(PNG_INCLUDE_DIR "${png_SOURCE_DIR}" "${png_BINARY_DIR}")
endif()

set(TABGRAPH_GLTF_PNG_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/PNG.cpp)
add_library(TabGraph-Assets-Parsers-PNG ${TABGRAPH_GLTF_PNG_SOURCE_FILES})
target_link_libraries(TabGraph-Assets-Parsers-PNG TabGraph::Assets ${PNG_LIBRARIES})
target_include_directories(TabGraph-Assets-Parsers-PNG PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${PNG_INCLUDE_DIR}")
set_target_properties(
  TabGraph-Assets-Parsers-PNG
  PROPERTIES FOLDER "TabGraph/Assets/Parsers"
)

set(TABGRAPH_PARSER_LIBS ${TABGRAPH_PARSER_LIBS} TabGraph-Assets-Parsers-PNG PARENT_SCOPE)

#Mime-Extension
set(TABGRAPH_PARSER_MIME_EXTENSION0 ${TABGRAPH_PARSER_MIME_EXTENSION0} "image/x-png" PARENT_SCOPE)
set(TABGRAPH_PARSER_MIME_EXTENSION1 ${TABGRAPH_PARSER_MIME_EXTENSION1} ".png" PARENT_SCOPE)

set(TABGRAPH_PARSER_MIME_FUNCTION0 ${TABGRAPH_PARSER_MIME_FUNCTION0} "image/x-png" PARENT_SCOPE)
set(TABGRAPH_PARSER_MIME_FUNCTION1 ${TABGRAPH_PARSER_MIME_FUNCTION1} "ParsePNG" PARENT_SCOPE)