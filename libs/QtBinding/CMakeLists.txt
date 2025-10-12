find_package(Qt6 REQUIRED COMPONENTS Gui)

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/)
set(SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/)

set(HEADER_FILES ${HEADER_FILES} ${INCLUDE_DIR}/QWindow.hpp)

set(SOURCE_FILES ${SOURCE_FILES} ${SOURCE_DIR}/QWindow.cpp)

add_library(MSG-QtBinding ${SOURCE_FILES} ${HEADER_FILES})
add_library(MSG::QtBinding ALIAS MSG-QtBinding)
target_include_directories(MSG-QtBinding PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-QtBinding
  MSG::Renderer
  MSG::OGLContext::Implementation
  Qt6::Gui)

set_target_properties(
  MSG-QtBinding
  PROPERTIES FOLDER "MSG"
)