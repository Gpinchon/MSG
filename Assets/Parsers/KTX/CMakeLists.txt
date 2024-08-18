cmake_minimum_required(VERSION 3.19)

FetchContent_Declare(
    EGL_REGISTRY
    GIT_TAG        main
    GIT_REPOSITORY https://github.com/KhronosGroup/EGL-Registry.git
)
FetchContent_MakeAvailable(EGL_REGISTRY)
set(EGL_INCLUDE_DIRS
  "${egl_registry_SOURCE_DIR}/api")

FetchContent_Declare(
    GL_REGISTRY
    GIT_TAG        main
    GIT_REPOSITORY https://github.com/KhronosGroup/OpenGL-Registry/
)
FetchContent_MakeAvailable(GL_REGISTRY)
set(GL_INCLUDE_DIRS
  "${gl_registry_SOURCE_DIR}/api")


set(TABGRAPH_BMP_DATA_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/KTX.cpp)

add_library(TabGraph-Assets-Parsers-KTX ${TABGRAPH_BMP_DATA_SOURCE_FILES})
target_link_libraries(TabGraph-Assets-Parsers-KTX TabGraph::Assets)
target_include_directories(TabGraph-Assets-Parsers-KTX PRIVATE
  "${GL_INCLUDE_DIRS}"
  "${EGL_INCLUDE_DIRS}")

set_target_properties(
  TabGraph-Assets-Parsers-KTX
  PROPERTIES FOLDER "TabGraph/Assets/Parsers"
)

set(TABGRAPH_PARSER_LIBS ${TABGRAPH_PARSER_LIBS} TabGraph-Assets-Parsers-KTX PARENT_SCOPE)

set(TABGRAPH_PARSER_MIME_EXTENSION0 ${TABGRAPH_PARSER_MIME_EXTENSION0} "image/ktx" PARENT_SCOPE)
set(TABGRAPH_PARSER_MIME_EXTENSION1 ${TABGRAPH_PARSER_MIME_EXTENSION1} ".ktx" PARENT_SCOPE)

set(TABGRAPH_PARSER_MIME_FUNCTION0 ${TABGRAPH_PARSER_MIME_FUNCTION0} "image/ktx" PARENT_SCOPE)
set(TABGRAPH_PARSER_MIME_FUNCTION1 ${TABGRAPH_PARSER_MIME_FUNCTION1} "ParseKTX" PARENT_SCOPE)
