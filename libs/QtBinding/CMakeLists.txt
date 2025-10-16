set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/)
set(SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/)

set(HEADER_FILES
  ${INCLUDE_DIR}/QtWindow.hpp
  ${INCLUDE_DIR}/QtItem.hpp)

set(SOURCE_FILES
  ${SOURCE_DIR}/QtWindow.cpp
  ${SOURCE_DIR}/QtItem.cpp)

if (WIN32)
  set(SOURCE_FILES ${SOURCE_FILES}
    ${SOURCE_DIR}/Win32Context.cpp)
elseif(UNIX)
  set(SOURCE_FILES ${SOURCE_FILES}
    ${SOURCE_DIR}/UnixContext.cpp)
endif(WIN32)

if (MSG_RENDERER_BACKEND MATCHES "OpenGL")
  set(SOURCE_FILES ${SOURCE_FILES}
    ${SOURCE_DIR}/OGLBinding.cpp)
endif (MSG_RENDERER_BACKEND MATCHES "OpenGL")

qt_add_library(MSG-QtBinding STATIC ${SOURCE_FILES} ${HEADER_FILES})
add_library(MSG::QtBinding ALIAS MSG-QtBinding)
target_include_directories(MSG-QtBinding PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-QtBinding
  MSG::Renderer
  MSG::OGLContext::Implementation
  Qt6::Quick)

set_target_properties(
  MSG-QtBinding
  PROPERTIES FOLDER "MSG"
)