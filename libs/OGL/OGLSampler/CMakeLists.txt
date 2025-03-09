set(MSG_OGL_TEXTURE_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/OGLSampler.hpp)
set(MSG_OGL_TEXTURE_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/OGLSampler.cpp)

add_library(MSG-OGLSampler STATIC ${MSG_OGL_TEXTURE_HEADER_FILES} ${MSG_OGL_TEXTURE_SOURCE_FILES})
add_library(MSG::OGLSampler ALIAS MSG-OGLSampler)
target_include_directories(MSG-OGLSampler PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-OGLSampler
  MSG::OGLContext
  glm::glm)
set_target_properties(
  MSG-OGLSampler
  PROPERTIES FOLDER "MSG/OGL")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
  GroupSources(inline)
endif()