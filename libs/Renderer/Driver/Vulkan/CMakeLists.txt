cmake_policy(SET CMP0072 NEW)

Fetch_VulkanSDK()

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Renderer/VK)
set(SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Renderer/VK)

set(MSG_RENDERER_IMPL_HEADER_FILES
  ${INCLUDE_DIR}/Device.hpp
  ${INCLUDE_DIR}/Instance.hpp
  ${INCLUDE_DIR}/PhysicalDevice.hpp
  ${INCLUDE_DIR}/Renderer.hpp
  ${INCLUDE_DIR}/RenderBuffer.hpp)

set(MSG_RENDERER_IMPL_SOURCE_FILES
  ${SOURCE_DIR}/Device.cpp
  ${SOURCE_DIR}/Instance.cpp
  ${SOURCE_DIR}/PhysicalDevice.cpp
  ${SOURCE_DIR}/Renderer.cpp
  ${SOURCE_DIR}/RenderBuffer.cpp)

add_library(MSG-Renderer-Implementation STATIC
  ${MSG_RENDERER_IMPL_SOURCE_FILES}
  ${MSG_RENDERER_IMPL_HEADER_FILES})
add_library(MSG::Renderer::Implementation ALIAS MSG-Renderer-Implementation)
target_include_directories(MSG-Renderer-Implementation PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-Renderer-Implementation
  MSG::Renderer::Interface
  MSG::Renderer::Shaders
  MSG::Renderer::ShadersLibrary
  MSG::VKUtils
  MSG::BRDF
  MSG::Scene
  MSG::Material)

set_target_properties(
  MSG-Renderer-Implementation
  PROPERTIES FOLDER "MSG/Renderer")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
  GroupSources(inline)
endif()
