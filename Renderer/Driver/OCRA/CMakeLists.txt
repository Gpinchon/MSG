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

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/Renderer/OCRA)
set(SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/src/Renderer/OCRA)

set(TABGRAPH_RENDERER_IMPL_HEADER_FILES
  ${INCLUDE_DIR}/RenderBuffer.hpp
  ${INCLUDE_DIR}/Renderer.hpp
  ${INCLUDE_DIR}/Component/MeshData.hpp
)

set(TABGRAPH_RENDERER_IMPL_SOURCE_FILES
  ${SOURCE_DIR}/Renderer.cpp
  ${SOURCE_DIR}/SwapChain.cpp
)

add_library(TabGraph-Renderer-Implementation ${TABGRAPH_RENDERER_IMPL_SOURCE_FILES} ${TABGRAPH_RENDERER_IMPL_HEADER_FILES})
add_library(TabGraph::Renderer::Implementation ALIAS TabGraph-Renderer-Implementation)
target_include_directories(TabGraph-Renderer-Implementation PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(TabGraph-Renderer-Implementation
  TabGraph::SG
  TabGraph::Renderer::Interface
  OCRA::API::Interface
  OCRA::API::Implementation
  OCRA::SwapChain::Interface
  OCRA::SwapChain::Implementation)

set_target_properties(
  TabGraph-Renderer-Implementation
  PROPERTIES FOLDER "TabGraph"
)