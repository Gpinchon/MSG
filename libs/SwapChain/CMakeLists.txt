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
  PROPERTIES FOLDER "MSG/SwapChain")

if (MSG_RENDERER_BACKEND STREQUAL "OpenGL")
  add_subdirectory(Driver/OpenGL/Shaders)
  add_subdirectory(Driver/OpenGL)
endif (MSG_RENDERER_BACKEND STREQUAL "OpenGL")

add_library(MSG-SwapChain INTERFACE)
add_library(MSG::SwapChain ALIAS MSG-SwapChain)
target_link_libraries(MSG-SwapChain INTERFACE
    MSG::SwapChain::Interface
    MSG::SwapChain::Implementation)

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
  GroupSources(inline)
endif()