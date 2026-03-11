macro(Fetch_DotnetRuntime)
  FetchContent_Declare(
    DOTNET_RUNTIME
    GIT_REPOSITORY https://github.com/dotnet/runtime.git
    GIT_TAG        v10.0.3
    GIT_SHALLOW    1
  )
  FetchContent_GetProperties(DOTNET_RUNTIME)
  if (NOT dotnet_runtime_POPULATED)
    message(STATUS "Fetching Dotnet runtime from Git repo")
    FetchContent_Populate(DOTNET_RUNTIME)
    configure_file(
      "${dotnet_runtime_SOURCE_DIR}/src/coreclr/pal/inc/rt/sal.h"
      "${CMAKE_BINARY_DIR}/external/include/sal.h" 
      COPYONLY)
  endif (NOT dotnet_runtime_POPULATED)
endmacro()