set(HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/VolumetricFog.hpp)

set(SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/VolumetricFog.cpp)

add_library(MSG-VolumetricFog STATIC ${HEADER_FILES} ${SOURCE_FILES})
add_library(MSG::VolumetricFog ALIAS MSG-VolumetricFog)
target_include_directories(MSG-VolumetricFog PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-VolumetricFog PUBLIC MSG::Image)

set_target_properties(
  MSG-VolumetricFog
  PROPERTIES FOLDER "MSG")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
endif()
