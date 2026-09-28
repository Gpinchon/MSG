function(Fetch_DirectXHeaders)
  find_package(DirectX-Headers CONFIG QUIET)
  if(directxheaders_FOUND)
    return()
  endif()
  MSG_InstallExternalPackage(DirectX-Headers
    https://github.com/microsoft/DirectX-Headers.git v1.619.1
    -DDXHEADERS_INSTALL:option=ON
    -DDXHEADERS_BUILD_TEST:option=OFF
    -DDXHEADERS_BUILD_GOOGLE_TEST:option=OFF)
  find_package(DirectX-Headers REQUIRED CONFIG PATHS "${MSG_EXTERNAL_PATH}" NO_DEFAULT_PATH)
endfunction()
