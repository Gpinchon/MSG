include(FetchContent)

macro(Fetch_SDL2)
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
endmacro()