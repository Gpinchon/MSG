macro(Fetch_STBImage)
  FetchContent_Declare(
  STB
  GIT_REPOSITORY https://github.com/nothings/stb.git
  )
  FetchContent_MakeAvailable(STB)
  set(STB_INCLUDE_DIR "${stb_SOURCE_DIR}")
endmacro()