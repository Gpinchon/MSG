function(BuildDirectXMath a_BuildType)
  message("Building DirectXMath::${a_BuildType}")
  execute_process(
    COMMAND ${CMAKE_COMMAND}
      -G ${CMAKE_GENERATOR}
      -S ${directxmath_SOURCE_DIR}
      -B ${directxmath_BINARY_DIR}
      --install-prefix ${CMAKE_BINARY_DIR}/external)
  execute_process(
    COMMAND ${CMAKE_COMMAND}
      --build ${directxmath_BINARY_DIR}
      --config ${a_BuildType})
  message("Installing DirectXMath::${a_BuildType} to ${CMAKE_BINARY_DIR}/external")
  execute_process(
    COMMAND ${CMAKE_COMMAND}
      --install ${directxmath_BINARY_DIR}
      --config ${a_BuildType})
endfunction()

macro(Fetch_DirectXMath)
  find_package(directxmath CONFIG QUIET)
  if(NOT directxmath_FOUND)
    message(STATUS "Fetching DirectXMath from Git repo")
    FetchContent_Declare(
      DIRECTXMATH
      GIT_REPOSITORY https://github.com/microsoft/DirectXMath.git
      GIT_TAG        apr2025
    )
    FetchContent_GetProperties(DIRECTXMATH)
    if (NOT directxmath_POPULATED)
      Fetch_DotnetRuntime()
      FetchContent_Populate(DIRECTXMATH)
      BuildDirectXMath(Release)
    endif (NOT directxmath_POPULATED)
  endif(NOT directxmath_FOUND)
endmacro()