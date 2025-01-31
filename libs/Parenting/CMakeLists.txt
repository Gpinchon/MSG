Fetch_GLM()

set(MSG_PARENTING_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Children.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Parent.hpp)

add_library(MSG-Parenting INTERFACE ${MSG_PARENTING_HEADER_FILES})
add_library(MSG::Parenting ALIAS MSG-Parenting)
target_include_directories(MSG-Parenting INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-Parenting INTERFACE MSG::ECS)

set_target_properties(
  MSG-Parenting
  PROPERTIES FOLDER "MSG")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
endif()
