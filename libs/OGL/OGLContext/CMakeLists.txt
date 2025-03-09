Fetch_GLEW()

set(MSG_OGL_CONTEXT_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/OGLContext.hpp)
set(MSG_OGL_CONTEXT_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/OGLContext.cpp)

add_library(MSG-OGLContext-Interface STATIC ${MSG_OGL_CONTEXT_HEADER_FILES} ${MSG_OGL_CONTEXT_SOURCE_FILES})
add_library(MSG::OGLContext::Interface ALIAS MSG-OGLContext-Interface)
target_compile_definitions(MSG-OGLContext-Interface PUBLIC GLEW_NO_GLU)
target_compile_definitions(MSG-OGLContext-Interface PUBLIC GLEW_STATIC)
target_include_directories(MSG-OGLContext-Interface PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-OGLContext-Interface
  MSG::Threads
  ${OPENGL_LIBRARIES})
set_target_properties(
  MSG-OGLContext-Interface
  PROPERTIES FOLDER "MSG/OGL")

if (WIN32)
  add_subdirectory(Driver/Win32)
elseif(UNIX)
  add_subdirectory(Driver/Unix)
endif(WIN32)

add_library(MSG-OGLContext INTERFACE)
add_library(MSG::OGLContext ALIAS MSG-OGLContext)
target_link_libraries(MSG-OGLContext INTERFACE
    MSG::OGLContext::Interface
    MSG::OGLContext::Implementation)

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
  GroupSources(inline)
endif()