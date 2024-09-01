cmake_minimum_required(VERSION 3.19)
cmake_policy(SET CMP0074 NEW)

# Fetch SDL
FetchContent_Declare(
  SDL
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG        release-2.28.3)
option(SDL_SHARED "" OFF)
option(SDL_STATIC "" ON)
option(SDL2_DISABLE_UNINSTALL  "Disable uninstallation of SDL2" ON)
option(SDL2_DISABLE_INSTALL    "Disable installation of SDL2" ON)
FetchContent_MakeAvailable(SDL)
message(STATUS "Fetched SDL to ${sdl_SOURCE_DIR}")
set_subdirectory_folder("3rdparty/SDL" ${sdl_SOURCE_DIR})
# Fetch SDL


set(TABGRAPH_TEST_SG_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

add_executable(TabGraph-UnitTests-SceneRenderer
  ${TABGRAPH_TEST_SG_SRC})
target_include_directories(TabGraph-UnitTests-SceneRenderer PRIVATE
  ${sdl_SOURCE_DIR}/include
)
target_link_libraries(TabGraph-UnitTests-SceneRenderer
  SDL2::SDL2main
  SDL2::SDL2-static
  TabGraph::SG
  TabGraph::Renderer::Interface
  TabGraph::Renderer::Implementation
  TabGraph::Assets::Parsers)

if(MSVC_IDE)
  set_property(TARGET TabGraph-UnitTests-SceneRenderer PROPERTY
    VS_DEBUGGER_ENVIRONMENT "${OCRA_DEBUGGER_ENV}")
  # Run macro
  GroupSources(src)
  GroupSources(include)
endif()
