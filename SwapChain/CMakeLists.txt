set(TABGRAPH_SWAPCHAIN_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SwapChain/Handles.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SwapChain/Structs.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SwapChain/SwapChain.hpp
)

add_library(TabGraph-SwapChain-Interface INTERFACE ${TABGRAPH_SWAPCHAIN_HEADER_FILES})
add_library(TabGraph::SwapChain::Interface ALIAS TabGraph-SwapChain-Interface)
target_include_directories(TabGraph-SwapChain-Interface PUBLIC INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(TabGraph-SwapChain-Interface INTERFACE
  TabGraph::Renderer::Interface)
set_target_properties(
  TabGraph-SwapChain-Interface
  PROPERTIES FOLDER "TabGraph")

set(TABGRAPH_SWAPCHAIN_IMPL "OpenGL" CACHE STRING
  "API Impl used at compile-time")
set(TABGRAPH_SWAPCHAIN_IMPL_VALUES
  "OpenGL;")
set_property(CACHE TABGRAPH_SWAPCHAIN_IMPL PROPERTY STRINGS ${TABGRAPH_SWAPCHAIN_IMPL_VALUES})

add_compile_definitions(
  TABGRAPH_SWAPCHAIN_OpenGL=0
  TABGRAPH_SWAPCHAIN_IMPL=TABGRAPH_SWAPCHAIN_${TABGRAPH_SWAPCHAIN_IMPL})

if (TABGRAPH_SWAPCHAIN_IMPL STREQUAL "OpenGL")
  add_subdirectory(Driver/OpenGL/Shaders)
endif (TABGRAPH_SWAPCHAIN_IMPL STREQUAL "OpenGL")

if (TABGRAPH_SWAPCHAIN_IMPL STREQUAL "OpenGL")
  add_subdirectory(Driver/OpenGL)
endif (TABGRAPH_SWAPCHAIN_IMPL STREQUAL "OpenGL")

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(src)
  GroupSources(include)
  GroupSources(inline)
endif()