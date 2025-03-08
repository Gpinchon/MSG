cmake_minimum_required(VERSION 3.19)
cmake_policy(SET CMP0074 NEW)

set(SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
add_executable(MSG-UnitTests-Sponza ${SOURCES})

file(GLOB_RECURSE TEXTURES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/scene/textures/*.jpg
  ${CMAKE_CURRENT_SOURCE_DIR}/scene/textures/*.png)

set(RESOURCES
  scene/Sponza.gltf
  scene/Sponza.bin
  scene/env.jpg
  scene/skybox.hdr
  ${TEXTURES})
AddRuntimeFiles(MSG-UnitTests-Sponza ${RESOURCES})

target_include_directories(MSG-UnitTests-Sponza PRIVATE
  ${sdl_SOURCE_DIR}/include)
target_link_libraries(MSG-UnitTests-Sponza
  MSG::Scene
  MSG::ShapeGenerator
  MSG::Renderer
  MSG::Window
  MSG::Keyboard
  MSG::Mouse
  MSG::Assets::Parsers)

if(MSVC_IDE)
  # Run macro
  GroupSources(src)
  GroupSources(include)
endif()
