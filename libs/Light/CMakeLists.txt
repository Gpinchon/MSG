set(MSG_LIGHT_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Light/LightCookie.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Light/PunctualLight.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Light/ShadowCaster.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Light/Sky.hpp)

set(MSG_LIGHT_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Light/IBL.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Light/Sky.cpp)

add_library(MSG-Light ${MSG_LIGHT_SOURCE_FILES} ${MSG_LIGHT_HEADER_FILES})
add_library(MSG::Light ALIAS MSG-Light)
target_include_directories(MSG-Light PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-Light MSG::Texture)

set_target_properties(
  MSG-Light
  PROPERTIES FOLDER "MSG")

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(src)
  GroupSources(include)
endif()
