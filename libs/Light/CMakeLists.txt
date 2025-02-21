set(MSG_LIGHT_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Light/LightCookie.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Light/PunctualLight.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Light/ShadowCaster.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Light/Sky.hpp)

set(MSG_LIGHT_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Light/PunctualLight.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Light/IBL.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Light/Sky.cpp)

add_library(MSG-Light ${MSG_LIGHT_SOURCE_FILES} ${MSG_LIGHT_HEADER_FILES})
add_library(MSG::Light ALIAS MSG-Light)
target_include_directories(MSG-Light PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-Light MSG::Texture)

set_target_properties(
  MSG-Light
  PROPERTIES FOLDER "MSG")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
endif()
