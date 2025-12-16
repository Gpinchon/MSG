set(MSG_OGL_TEXTURE_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/OGLBindlessTextureSampler.hpp)
set(MSG_OGL_TEXTURE_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/OGLBindlessTextureSampler.cpp)

add_library(MSG-OGLBindlessTextureSampler STATIC ${MSG_OGL_TEXTURE_HEADER_FILES} ${MSG_OGL_TEXTURE_SOURCE_FILES})
add_library(MSG::OGLBindlessTextureSampler ALIAS MSG-OGLBindlessTextureSampler)
target_include_directories(MSG-OGLBindlessTextureSampler PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-OGLBindlessTextureSampler
  MSG::OGLContext
  MSG::OGLTexture
  MSG::OGLSampler
  glm::glm)
set_target_properties(
  MSG-OGLBindlessTextureSampler
  PROPERTIES FOLDER "MSG/OGL")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
  GroupSources(inline)
endif()