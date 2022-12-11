# Fetch OCRA
FetchContent_Declare(
  OCRA
  GIT_REPOSITORY https://github.com/Gpinchon/OCRA.git
  GIT_TAG        origin/main
)
FetchContent_MakeAvailable(OCRA)
message(STATUS "Fetched OCRA to ${ocra_SOURCE_DIR}")
set_subdirectory_folder("3rdparty/OCRA" ${ocra_SOURCE_DIR})
# Fetch OCRA

set(TABGRAPH_RENDERER_HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Renderer.hpp
)

set(TABGRAPH_RENDERER_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Renderer.cpp
)

add_library(TabGraph-Renderer ${TABGRAPH_RENDERER_SOURCE_FILES} ${TABGRAPH_RENDERER_HEADER_FILES})
add_library(TabGraph::Renderer ALIAS TabGraph-Renderer)
target_include_directories(TabGraph-Renderer PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(TabGraph-Renderer TabGraph::SG OCRA::API::Interface OCRA::API::Implementation)

set_target_properties(
  TabGraph-Renderer
  PROPERTIES FOLDER "TabGraph"
)