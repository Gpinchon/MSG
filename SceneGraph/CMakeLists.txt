Fetch_GLM()
Fetch_FasTC()

set(TABGRAPH_SG_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Component/Light/PunctualLight.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Component/Light/Sky.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Component/Camera.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Component/Children.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Component/Mesh.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Component/MeshSkin.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Component/Name.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Component/Parent.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Component/Projection.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Component/Transform.hpp

  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Core/Buffer/Accessor.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Core/Buffer/Buffer.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Core/Buffer/Iterator.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Core/Buffer/View.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Core/Image/Cubemap.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Core/Image/Image.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Core/Image/Image1D.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Core/Image/Image2D.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Core/Image/Image3D.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Core/Image/Pixel.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Core/Material/Extension/Extension.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Core/Material/Extension/Base.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Core/Material/Extension/MetallicRoughness.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Core/Material/Extension/Sheen.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Core/Material/Extension/SpecularGlossiness.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Core/Material/Extension/Unlit.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Core/Material/TextureInfo.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Core/Texture/Sampler.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Core/Texture/Texture.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Core/Texture/TextureSampler.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Core/BoundingVolume.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Core/Inherit.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Core/Material.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Core/Primitive.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Core/Object.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Core/Property.hpp

  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Entity/Light/PunctualLight.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Entity/Light/Sky.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Entity/Node.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Entity/NodeGroup.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Entity/Camera.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Entity/Entity.hpp

  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Scene/Animation/Channel.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Scene/Animation/Interpolator.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Scene/Animation.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Scene/Scene.hpp

  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/ShapeGenerator/Capsule.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/ShapeGenerator/Cube.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/ShapeGenerator/Plane.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/ShapeGenerator/Sphere.hpp

  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/Common.hpp)

set(TABGRAPH_SG_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Component/Light/LightIBL.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Component/Mesh.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Core/Buffer/Accessor.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Core/Buffer/Buffer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Core/Buffer/View.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Core/Image/Cubemap.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Core/Image/Image.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Core/Image/Image1D.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Core/Image/Image2D.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Core/Image/Image3D.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Core/Image/Pixel.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Core/Texture/Texture.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Core/Object.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Core/Primitive.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Core/Material.cpp
  

  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Entity/Light/PunctualLight.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Entity/Light/Sky.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Entity/Node.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Entity/NodeGroup.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Entity/Entity.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Entity/Camera.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Scene/Scene.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Scene/Animation.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/ShapeGenerator/Capsule.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/ShapeGenerator/Cube.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/ShapeGenerator/Sphere.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/src/SG/Common.cpp)

add_library(TabGraph-SG ${TABGRAPH_SG_SOURCE_FILES} ${TABGRAPH_SG_HEADER_FILES})
add_library(TabGraph::SG ALIAS TabGraph-SG)
target_include_directories(TabGraph-SG PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  ${FASTCBASE_INCLUDES}
  ${FASTCCORE_INCLUDES})
target_link_libraries(TabGraph-SG
  TabGraph::Tools
  TabGraph::ECS
  glm::glm
  FasTCBase
  FasTCCore
  FasTCIO)

set_target_properties(
  TabGraph-SG
  PROPERTIES FOLDER "TabGraph")

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(src)
  GroupSources(include)
endif()
