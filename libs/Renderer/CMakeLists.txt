set(MSG_RENDERER_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Renderer/Enums.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Renderer/Handles.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Renderer/RenderBuffer.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Renderer/Structs.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Renderer.hpp)

add_library(MSG-Renderer-Interface INTERFACE ${MSG_RENDERER_HEADER_FILES})
add_library(MSG::Renderer::Interface ALIAS MSG-Renderer-Interface)
target_include_directories(MSG-Renderer-Interface PUBLIC INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-Renderer-Interface INTERFACE
  MSG::Scene)
set_target_properties(
  MSG-Renderer-Interface
  PROPERTIES FOLDER "MSG/Renderer")

if (MSG_RENDERER_BACKEND MATCHES "OpenGL")
  add_subdirectory(Driver/OpenGL/Shaders)
  add_subdirectory(Driver/OpenGL)
elseif (MSG_RENDERER_BACKEND MATCHES "Vulkan")
  add_subdirectory(Driver/Vulkan/Shaders)
  add_subdirectory(Driver/Vulkan)
elseif (MSG_RENDERER_BACKEND MATCHES "DirectX")
  add_subdirectory(Driver/DirectX/Shaders)
  add_subdirectory(Driver/DirectX)
endif (MSG_RENDERER_BACKEND MATCHES "OpenGL")

add_subdirectory(ShaderLibrary)

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