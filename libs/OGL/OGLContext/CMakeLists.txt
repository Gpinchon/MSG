Fetch_GLEW()

set(MSG_OGL_CONTEXT_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/OGLContext.hpp)
set(MSG_OGL_CONTEXT_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/OGLContext.cpp)

add_library(MSG-OGLContext-Interface STATIC ${MSG_OGL_CONTEXT_HEADER_FILES} ${MSG_OGL_CONTEXT_SOURCE_FILES})
add_library(MSG::OGLContext::Interface ALIAS MSG-OGLContext-Interface)
target_include_directories(MSG-OGLContext-Interface PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-OGLContext-Interface
  MSG::Tools
  GLEW::GLEW)
set_target_properties(
  MSG-OGLContext-Interface
  PROPERTIES FOLDER "MSG/OGL")

if (WIN32)
  add_subdirectory(Driver/Win32)
elseif(UNIX)
  add_subdirectory(Driver/Unix)
endif(WIN32)

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(src)
  GroupSources(include)
  GroupSources(inline)
endif()