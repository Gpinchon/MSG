Fetch_GLM()
Fetch_FasTC()

set(MSG_CORE_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Buffer/Accessor.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Buffer/Buffer.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Buffer/Iterator.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Buffer/View.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Image/Cubemap.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Image/Image.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Image/Image1D.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Image/Image2D.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Image/Image3D.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Image/Pixel.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Image/PixelSizedFormat.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Image/PixelSizedFormatHelper.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Image/PixelUnsizedFormat.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Light/LightCookie.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Light/PunctualLight.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Light/ShadowCaster.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Light/Sky.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Material/Extension/Extension.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Material/Extension/Base.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Material/Extension/MetallicRoughness.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Material/Extension/Sheen.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Material/Extension/SpecularGlossiness.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Material/Extension/Unlit.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Material/TextureInfo.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Shapes/Box.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Shapes/Plane.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Shapes/Sphere.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Texture/Sampler.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Texture/Texture.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Texture/TextureSampler.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/BoundingVolume.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Camera.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Children.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/DataType.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Frustum.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Inherit.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Material.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Mesh.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/MeshSkin.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Name.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Object.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Orientation.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Parent.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Primitive.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Projection.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Property.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Transform.hpp)

set(MSG_CORE_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/Buffer/Accessor.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/Buffer/Buffer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/Buffer/View.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/Image/Cubemap.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/Image/Image.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/Image/Image1D.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/Image/Image2D.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/Image/Image3D.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/Image/Pixel.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/Light/LightIBL.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/Texture/Texture.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/Material.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/Mesh.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/Object.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/Orientation.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/Primitive.cpp)

add_library(MSG-Core ${MSG_CORE_SOURCE_FILES} ${MSG_CORE_HEADER_FILES})
add_library(MSG::Core ALIAS MSG-Core)
target_include_directories(MSG-Core PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  ${FASTCBASE_INCLUDES}
  ${FASTCCORE_INCLUDES})
target_link_libraries(MSG-Core
  MSG::ECS
  glm::glm
  FasTCBase
  FasTCCore
  FasTCIO)

set_target_properties(
  MSG-Core
  PROPERTIES FOLDER "MSG")

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(src)
  GroupSources(include)
endif()
