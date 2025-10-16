cmake_minimum_required(VERSION 3.19)
cmake_policy(SET CMP0074 NEW)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.10 REQUIRED COMPONENTS Quick)

qt_add_executable(MSG-UnitTests-QtBinding 
  ${CMAKE_CURRENT_SOURCE_DIR}/res/application.qrc
  ${CMAKE_CURRENT_SOURCE_DIR}/include/QtBindingTestItem.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/QtBindingTestItem.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
target_include_directories(MSG-UnitTests-QtBinding PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include/)
target_link_libraries(MSG-UnitTests-QtBinding PUBLIC
  MSG::Scene
  MSG::ShapeGenerator
  MSG::Renderer
  MSG::Assets::Parsers
  MSG::QtBinding
  Qt6::Quick)

if(MSVC_IDE)
  # Run macro
  GroupSources(src)
  GroupSources(include)
endif()
