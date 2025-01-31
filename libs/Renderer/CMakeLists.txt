set(MSG_RENDERER_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Renderer/Handles.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Renderer/RenderBuffer.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Renderer/Structs.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Renderer.hpp)

add_library(MSG-Renderer-Interface INTERFACE ${MSG_RENDERER_HEADER_FILES})
add_library(MSG::Renderer::Interface ALIAS MSG-Renderer-Interface)
target_include_directories(MSG-Renderer-Interface PUBLIC INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-Renderer-Interface INTERFACE MSG::Scene)
set_target_properties(
  MSG-Renderer-Interface
  PROPERTIES FOLDER "MSG/Renderer")

set(MSG_RENDERER_IMPL "OpenGL" CACHE STRING
  "API Impl used at compile-time")
set(MSG_RENDERER_IMPL_VALUES
  "OpenGL;Vulkan;DirectX")
set_property(CACHE MSG_RENDERER_IMPL PROPERTY STRINGS ${MSG_RENDERER_IMPL_VALUES})

add_compile_definitions(
  MSG_RENDERER_OpenGL=0
  MSG_RENDERER_Vulkan=1
  MSG_RENDERER_DirectX=2
  MSG_RENDERER_IMPL=MSG_RENDERER_${MSG_RENDERER_IMPL})

if (MSG_RENDERER_IMPL MATCHES "OpenGL")
  add_subdirectory(Driver/OpenGL/Shaders)
elseif (MSG_RENDERER_IMPL MATCHES "Vulkan")
  add_subdirectory(Driver/Vulkan/Shaders)
  elseif (MSG_RENDERER_IMPL MATCHES "DirectX")
  add_subdirectory(Driver/DirectX/Shaders)
endif (MSG_RENDERER_IMPL MATCHES "OpenGL")

add_subdirectory(ShaderLibrary)

if (MSG_RENDERER_IMPL MATCHES "OpenGL")
  add_subdirectory(Driver/OpenGL)
elseif (MSG_RENDERER_IMPL MATCHES "Vulkan")
  add_subdirectory(Driver/Vulkan)
  elseif (MSG_RENDERER_IMPL MATCHES "DirectX")
  add_subdirectory(Driver/DirectX)
endif (MSG_RENDERER_IMPL MATCHES "OpenGL")

add_library(MSG-Renderer INTERFACE)
add_library(MSG::Renderer ALIAS MSG-Renderer)
target_link_libraries(MSG-Renderer INTERFACE
    MSG::Renderer::Interface
    MSG::Renderer::Implementation)

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
  GroupSources(inline)
endif()