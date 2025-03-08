include(FetchContent)

macro(Fetch_SDL2)
  set(SDL2_DISABLE_SDL2MAIN ON CACHE BOOL "" FORCE)
  set(SDL2_DISABLE_UNINSTALL ON CACHE BOOL "" FORCE)
  set(SDL2_DISABLE_INSTALL ON CACHE BOOL "" FORCE)
  
  find_package(SDL2 2.32.0 QUIET COMPONENTS SDL2-static)
  if (NOT SDL2_FOUND)
    set(SDL_STATIC ON CACHE BOOL "" FORCE)
    set(SDL_SHARED OFF CACHE BOOL "" FORCE)
    set(SDL_RENDER OFF CACHE BOOL "" FORCE)
    set(SDL_WAYLAND OFF CACHE BOOL "" FORCE)
    set(SDL_VULKAN OFF CACHE BOOL "" FORCE)
    set(SDL_DIRECTX OFF CACHE BOOL "" FORCE)
    set(SDL_OPENGL OFF CACHE BOOL "" FORCE)
    set(SDL_OPENGLES OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
      SDL
      GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
      GIT_TAG        release-2.32.0)
    FetchContent_MakeAvailable(SDL)
    message(STATUS "Fetched SDL to ${sdl_SOURCE_DIR}")
    set_subdirectory_folder("3rdparty/SDL" ${sdl_SOURCE_DIR})
  endif (NOT SDL2_FOUND)
endmacro()