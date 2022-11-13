include(FetchContent)

# Fetch GLM
FetchContent_Declare(
  GLM
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG        0.9.9.8
)
FetchContent_MakeAvailable(GLM)
message(STATUS "Fetched GLM to ${glm_SOURCE_DIR}")
set(glm_DIR "${glm_SOURCE_DIR}/cmake/glm")
set_subdirectory_folder("3rdparty/GLM" ${glm_SOURCE_DIR})
# Fetch GLM

set(TABGRAPH_SG_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Animation/Animation.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Animation/Channel.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Animation/Interpolator.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Buffer/Accessor.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Buffer/Buffer.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Buffer/Iterator.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Buffer/View.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Camera/Camera.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Camera/FPS.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Camera/Orbit.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Camera/Projection.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Inherit.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Object.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Property.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Image/Image.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Image/Pixel.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Light/Directional.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Light/Light.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Light/HDR.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Light/Point.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Light/Sky.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Light/Spot.hpp)

set(TABGRAPH_SG_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Animation/Animation.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Buffer/Accessor.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Buffer/View.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Camera/Camera.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Camera/FPS.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Camera/Orbit.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Camera/Projection.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/Object.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Image/Pixel.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Light/Directional.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Light/Light.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Light/HDR.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Light/Point.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Light/Sky.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Light/Spot.cpp)

add_library(TabGraph-SG ${TABGRAPH_SG_SOURCE_FILES} ${TABGRAPH_SG_HEADER_FILES})
add_library(TabGraph::SG ALIAS TabGraph-SG)
target_include_directories(TabGraph-SG PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(TabGraph-SG TabGraph::Tools glm::glm)

set_target_properties(
  TabGraph-SG
  PROPERTIES FOLDER "TabGraph"
)

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(src)
  GroupSources(include)
endif()
