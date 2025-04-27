function(BuildZLIB a_BuildType)
  message("Building ZLIB::${a_BuildType}")
  execute_process(
    COMMAND ${CMAKE_COMMAND}
      -G ${CMAKE_GENERATOR}
      -S ${zlib_SOURCE_DIR}
      -B ${zlib_BINARY_DIR}
      --install-prefix ${CMAKE_BINARY_DIR}/external)
  execute_process(
    COMMAND ${CMAKE_COMMAND}
      --build ${zlib_BINARY_DIR}
      --config ${a_BuildType})
  execute_process(
    COMMAND ${CMAKE_COMMAND}
      --install ${zlib_BINARY_DIR}
      --config ${a_BuildType})
endfunction()

macro(Fetch_ZLib)
  find_package(ZLIB QUIET)
  if(NOT ZLIB_FOUND)
    #if we don't have it already fetch it and install it in build directory
    FetchContent_Declare(
      ZLIB
      URL       https://www.zlib.net/current/zlib.tar.gz
    )
    FetchContent_GetProperties(ZLIB)
    if (NOT zlib_POPULATED)
      FetchContent_Populate(ZLIB)
      BuildZLIB(Release)
    endif (NOT zlib_POPULATED)
    option(ZLIB_USE_STATIC_LIBS "" TRUE)
    find_package(ZLIB REQUIRED)
  endif(NOT ZLIB_FOUND)
endmacro()