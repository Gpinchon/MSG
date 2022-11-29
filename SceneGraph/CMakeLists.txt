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
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Animation/Animation.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Animation/Channel.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Animation/Interpolator.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Buffer/Accessor.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Buffer/Buffer.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Buffer/Iterator.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Buffer/View.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Camera/Camera.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Camera/FPS.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Camera/Orbit.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Camera/Projection.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Core/Inherit.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Core/Object.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Core/Property.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Image/Image.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Image/Pixel.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Light/Directional.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Light/Light.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Light/HDR.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Light/Point.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Light/Sky.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Light/Spot.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Material/Material.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Material/Extension/MetallicRoughness.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Material/Extension/Sheen.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Material/Extension/SpecularGlossiness.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Node/Node.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Node/NodeGroup.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Node/Scene.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Node/Transform.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Shape/Generator/Capsule.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Shape/Geometry.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Shape/Mesh.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Shape/MeshSkin.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Shape/Shape.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Shape/Skybox.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Texture/Sampler.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Texture/Texture.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Texture/Texture2D.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Visitor/NodeVisitor.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Visitor/SearchVisitor.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Visitor/Visitor.hpp)

set(TABGRAPH_SG_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Animation/Animation.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Buffer/Accessor.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Buffer/Buffer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Buffer/View.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Camera/Camera.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Camera/FPS.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Camera/Orbit.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Camera/Projection.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Core/Object.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Image/Pixel.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Image/Image.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Light/Directional.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Light/Light.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Light/HDR.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Light/Point.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Light/Sky.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Light/Spot.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Material/Material.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Node/Node.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Node/NodeGroup.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Node/Scene.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Shape/Generator/Capsule.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Visitor/NodeVisitor.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Visitor/SearchVisitor.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Visitor/Visitor.cpp)

add_library(TabGraph-SG ${TABGRAPH_SG_SOURCE_FILES} ${TABGRAPH_SG_HEADER_FILES})
add_library(TabGraph::SG ALIAS TabGraph-SG)
target_include_directories(TabGraph-SG PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(TabGraph-SG TabGraph::Tools TabGraph::ECS glm::glm)

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
