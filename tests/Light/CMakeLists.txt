cmake_minimum_required(VERSION 3.19)
cmake_policy(SET CMP0074 NEW)

set(SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
add_executable(MSG-UnitTests-Light ${SOURCES})

set(RESOURCES
  scenes/Dirs.gltf
  scenes/Dirs.bin
  scenes/Points.gltf
  scenes/Points.bin
  scenes/Spots.gltf
  scenes/Spots.bin)
AddRuntimeFiles(MSG-UnitTests-Light ${RESOURCES})

target_include_directories(MSG-UnitTests-Light PRIVATE
  ${sdl_SOURCE_DIR}/include)
target_link_libraries(MSG-UnitTests-Light
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
