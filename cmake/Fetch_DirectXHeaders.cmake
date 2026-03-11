function(BuildDirectXHeaders)
  message("Building DirectXHeaders")
  execute_process(
    COMMAND ${CMAKE_COMMAND}
      -D DXHEADERS_INSTALL:option=ON
      -D DXHEADERS_BUILD_TEST:option=OFF
      -D DXHEADERS_BUILD_GOOGLE_TEST:option=OFF
      -G ${CMAKE_GENERATOR}
      -S ${directxheaders_SOURCE_DIR}
      -B ${directxheaders_BINARY_DIR}
      --install-prefix ${CMAKE_BINARY_DIR}/external)
  execute_process(
    COMMAND ${CMAKE_COMMAND}
      --build ${directxheaders_BINARY_DIR})
  message("Installing DirectXHeaders to ${CMAKE_BINARY_DIR}/external")
  execute_process(
    COMMAND ${CMAKE_COMMAND}
      --install ${directxheaders_BINARY_DIR})
endfunction()

macro(Fetch_DirectXHeaders)
  find_package(directxheaders CONFIG QUIET)
  if(NOT directxheaders_FOUND)
    message(STATUS "Fetching DirectXHeaders from Git repo")
    FetchContent_Declare(
      DIRECTXHEADERS
      GIT_REPOSITORY https://github.com/microsoft/DirectX-Headers.git
      GIT_TAG        v1.619.1
    )
    FetchContent_GetProperties(DIRECTXHEADERS)
    if (NOT directxheaders_POPULATED)
      FetchContent_Populate(DIRECTXHEADERS)
      BuildDirectXHeaders()
    endif (NOT directxheaders_POPULATED)
  endif(NOT directxheaders_FOUND)
endmacro()