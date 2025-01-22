set(MSG_SWAPCHAIN_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/SwapChain/Handles.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/SwapChain/Structs.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/SwapChain/SwapChain.hpp
)

add_library(MSG-SwapChain-Interface INTERFACE ${MSG_SWAPCHAIN_HEADER_FILES})
add_library(MSG::SwapChain::Interface ALIAS MSG-SwapChain-Interface)
target_include_directories(MSG-SwapChain-Interface PUBLIC INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-SwapChain-Interface INTERFACE
  MSG::Renderer::Interface)
set_target_properties(
  MSG-SwapChain-Interface
  PROPERTIES FOLDER "MSG")

set(MSG_SWAPCHAIN_IMPL "OpenGL" CACHE STRING
  "API Impl used at compile-time")
set(MSG_SWAPCHAIN_IMPL_VALUES
  "OpenGL;")
set_property(CACHE MSG_SWAPCHAIN_IMPL PROPERTY STRINGS ${MSG_SWAPCHAIN_IMPL_VALUES})

add_compile_definitions(
  MSG_SWAPCHAIN_OpenGL=0
  MSG_SWAPCHAIN_IMPL=MSG_SWAPCHAIN_${MSG_SWAPCHAIN_IMPL})

if (MSG_SWAPCHAIN_IMPL STREQUAL "OpenGL")
  add_subdirectory(Driver/OpenGL/Shaders)
endif (MSG_SWAPCHAIN_IMPL STREQUAL "OpenGL")

if (MSG_SWAPCHAIN_IMPL STREQUAL "OpenGL")
  add_subdirectory(Driver/OpenGL)
endif (MSG_SWAPCHAIN_IMPL STREQUAL "OpenGL")

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(src)
  GroupSources(include)
  GroupSources(inline)
endif()