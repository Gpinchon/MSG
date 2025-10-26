set(MSG_MATERIAL_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Material/Extension/Base.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Material/Extension/MetallicRoughness.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Material/Extension/Sheen.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Material/Extension/SpecularGlossiness.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Material/Extension/Unlit.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Material/Extension.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Material/TextureInfo.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Material.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/MaterialSet.hpp)

set(MSG_MATERIAL_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Material.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/MaterialSet.cpp)

add_library(MSG-Material ${MSG_MATERIAL_SOURCE_FILES} ${MSG_MATERIAL_HEADER_FILES})
add_library(MSG::Material ALIAS MSG-Material)
target_include_directories(MSG-Material PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-Material MSG::Image)

set_target_properties(
  MSG-Material
  PROPERTIES FOLDER "MSG")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
endif()
