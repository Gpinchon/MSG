function(MSG_RequireExternalPath)
  if(NOT DEFINED MSG_EXTERNAL_PATH)
    message(FATAL_ERROR "MSG_EXTERNAL_PATH must be set to the install prefix")
  endif()
endfunction()

function(MSG_Append_Debugger_Env debugger_env)
  set(current_env "$CACHE{MSG_DEBUGGER_ENV}")
  list(APPEND current_env ${debugger_env})
  list(REMOVE_DUPLICATES current_env)
  set(MSG_DEBUGGER_ENV "${current_env}" CACHE STRING "The debugging environment" FORCE)
  if(MSG_DEBUGGER_ENV)
    message(STATUS "MSG_DEBUGGER_ENV: ${MSG_DEBUGGER_ENV}")
    string(REPLACE ";" "\n" env_content "${MSG_DEBUGGER_ENV}")
    file(WRITE "${CMAKE_BINARY_DIR}/debugger.env" "${env_content}\n")
  endif()
endfunction()


function(MSG_InstallExternalPackage name repository tag)
  MSG_RequireExternalPath()
  set(stamp_dir "${MSG_EXTERNAL_PATH}/.msg-stamps")
  set(stamp "${stamp_dir}/${name}-${tag}")
  if(EXISTS "${stamp}")
    return()
  endif()

  FetchContent_Declare(
    ${name}_install
    GIT_REPOSITORY  ${repository}
    GIT_TAG         ${tag}
    GIT_SHALLOW     TRUE
    SOURCE_SUBDIR   "prevent_add_subdirectory" # download only: this subdirectory doesn't exist
  )
  FetchContent_MakeAvailable(${name}_install)
  FetchContent_GetProperties(${name}_install SOURCE_DIR source_dir BINARY_DIR binary_dir)

  message(STATUS "Installing ${name} (${tag}) to ${MSG_EXTERNAL_PATH}")
  execute_process(
    COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" ${ARGN} -S "${source_dir}" -B "${binary_dir}"
    COMMAND_ERROR_IS_FATAL ANY)
  execute_process(
    COMMAND ${CMAKE_COMMAND} --build ${binary_dir}
    COMMAND_ERROR_IS_FATAL ANY)
  execute_process(
    COMMAND ${CMAKE_COMMAND} --install "${binary_dir}" --prefix "${MSG_EXTERNAL_PATH}"
    COMMAND_ERROR_IS_FATAL ANY)

  file(MAKE_DIRECTORY "${stamp_dir}")
  file(WRITE "${stamp}" "")
endfunction()

# Add MSG_DEBUGGER_ENV to <target>'s debugger execution environment.
# MSVC_IDE : appends to VS_DEBUGGER_ENVIRONMENT, keeping the VC executable path on PATH
#            (VS expects one NAME=value entry per line, so the list is joined with newlines).
# Xcode    : appends to XCODE_SCHEME_ENVIRONMENT.
# Other generators have no per-target run environment. For CTest use
#   gtest_discover_tests(<target> PROPERTIES ENVIRONMENT "${MSG_DEBUGGER_ENV}")
function(MSG_Set_DebuggerEnvironment target)
  if(MSVC_IDE)
    string(REPLACE ";" "\n" env_lines "${MSG_DEBUGGER_ENV}")
    set_property(TARGET ${target} APPEND_STRING PROPERTY
      VS_DEBUGGER_ENVIRONMENT "PATH=$(VC_ExecutablePath_x64);%PATH%\n${env_lines}")
  elseif(XCODE)
    set_property(TARGET ${target} APPEND PROPERTY XCODE_SCHEME_ENVIRONMENT ${MSG_DEBUGGER_ENV})
  endif()
endfunction()