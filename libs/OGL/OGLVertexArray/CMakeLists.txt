set(MSG_OGL_TEXTURE_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/OGLIndexDescription.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/OGLVertexArray.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/OGLVertexAttributeDescription.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/OGLVertexBindingDescription.hpp)
set(MSG_OGL_TEXTURE_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/OGLVertexArray.cpp)

add_library(MSG-OGLVertexArray STATIC ${MSG_OGL_TEXTURE_HEADER_FILES} ${MSG_OGL_TEXTURE_SOURCE_FILES})
add_library(MSG::OGLVertexArray ALIAS MSG-OGLVertexArray)
target_include_directories(MSG-OGLVertexArray PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-OGLVertexArray
  MSG::OGLContext
  MSG::OGLBuffer)
set_target_properties(
  MSG-OGLVertexArray
  PROPERTIES FOLDER "MSG/OGL")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
  GroupSources(inline)
endif()