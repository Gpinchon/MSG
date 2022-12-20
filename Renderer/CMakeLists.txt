set(TABGRAPH_RENDERER_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Renderer/Handle.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Renderer/RenderBuffer.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Renderer/Renderer.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Renderer/SwapChain.hpp
)

add_library(TabGraph-Renderer-Interface INTERFACE ${TABGRAPH_RENDERER_HEADER_FILES})
add_library(TabGraph::Renderer::Interface ALIAS TabGraph-Renderer-Interface)
target_include_directories(TabGraph-Renderer-Interface PUBLIC INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(TabGraph-Renderer-Interface INTERFACE TabGraph::SG)

add_subdirectory(Driver/OCRA)

set_target_properties(
  TabGraph-Renderer-Interface
  PROPERTIES FOLDER "TabGraph"
)