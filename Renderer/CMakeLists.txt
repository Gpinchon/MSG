set(TABGRAPH_RENDERER_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Renderer/Handle.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Renderer/RenderBuffer.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Renderer/Renderer.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Renderer/Structs.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Renderer/SwapChain.hpp
)

add_library(TabGraph-Renderer-Interface INTERFACE ${TABGRAPH_RENDERER_HEADER_FILES})
add_library(TabGraph::Renderer::Interface ALIAS TabGraph-Renderer-Interface)
target_include_directories(TabGraph-Renderer-Interface PUBLIC INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(TabGraph-Renderer-Interface INTERFACE TabGraph::SG)

set(TABGRAPH_RENDERER_IMPL "OCRA" CACHE STRING
  "API Impl used at compile-time")
set(TABGRAPH_RENDERER_IMPL_VALUES
  "OCRA;OpenGL;Vulkan;DirectX")
set_property(CACHE TABGRAPH_RENDERER_IMPL PROPERTY STRINGS ${TABGRAPH_RENDERER_IMPL_VALUES})

add_compile_definitions(
  TABGRAPH_RENDERER_OCRA=0
  TABGRAPH_RENDERER_OpenGL=1
  TABGRAPH_RENDERER_Vulkan=2
  TABGRAPH_RENDERER_DirectX=3
  TABGRAPH_RENDERER_IMPL=TABGRAPH_RENDERER_${TABGRAPH_RENDERER_IMPL})

if (TABGRAPH_RENDERER_IMPL STREQUAL "OCRA")
  add_subdirectory(Driver/OCRA)
elseif (TABGRAPH_RENDERER_IMPL STREQUAL "OpenGL")
  add_subdirectory(Driver/OpenGL)
elseif (TABGRAPH_RENDERER_IMPL STREQUAL "Vulkan")
  add_subdirectory(Driver/Vulkan)
  elseif (TABGRAPH_RENDERER_IMPL STREQUAL "DirectX")
  add_subdirectory(Driver/DirectX)
endif (TABGRAPH_RENDERER_IMPL STREQUAL "OCRA")

set_target_properties(
  TabGraph-Renderer-Interface
  PROPERTIES FOLDER "TabGraph"
)

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(src)
  GroupSources(include)
  GroupSources(inline)
endif()