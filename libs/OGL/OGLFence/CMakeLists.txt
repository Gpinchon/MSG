set(MSG_OGL_TEXTURE_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/OGLFence.hpp)
set(MSG_OGL_TEXTURE_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/OGLFence.cpp)

add_library(MSG-OGLFence STATIC ${MSG_OGL_TEXTURE_HEADER_FILES} ${MSG_OGL_TEXTURE_SOURCE_FILES})
add_library(MSG::OGLFence ALIAS MSG-OGLFence)
target_include_directories(MSG-OGLFence PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
set_target_properties(
  MSG-OGLFence
  PROPERTIES FOLDER "MSG/OGL")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
endif()