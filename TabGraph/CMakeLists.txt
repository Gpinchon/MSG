cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0074 NEW)

#[[Rendering API]]
set(RenderingAPI "OpenGL" CACHE STRING
  "Rendering API used at compile-time")
set(RenderingAPIValues
  "OpenGL;Vulkan;DirectX")
set_property(CACHE RenderingAPI PROPERTY STRINGS ${RenderingAPIValues})
add_compile_definitions(RENDERINGAPI=${RenderingAPI})

#[[UI Sub-System]]
set(UISubSystem "ImGUI" CACHE STRING
  "UI Sub-System used at compile-time")
set(UISubSystemValues
  "ImGUI;NanoGUI")
set_property(CACHE UISubSystem PROPERTY STRINGS ${UISubSystemValues})
add_compile_definitions(UISUBSYSTEM=${UISubSystem})

#[[Media Library]]
set(MediaLibrary "SDL2" CACHE STRING
  "Media Library used at compile-time")
set(MediaLibraryValues
  "SDL2;GLFW;SFML")
set_property(CACHE MediaLibrary PROPERTY STRINGS ${MediaLibraryValues})
add_compile_definitions(MEDIALIBRARY=${MediaLibrary})

#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -DDEBUG_MOD")
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG_MOD")
set (CMAKE_CXX_FLAGS_RELEASE "-Ox")

set(CMAKE_DEBUG_TARGET_PROPERTIES INCLUDE_DIRECTORIES)
set(CMAKE_DEBUG_TARGET_PROPERTIES INTERFACE_INCLUDE_DIRECTORIES)

if (MediaLibrary STREQUAL "SDL2")
set(TABGRAPH_HEADER_FILES ${TABGRAPH_HEADER_FILES}
	${CMAKE_CURRENT_SOURCE_DIR}/include/Driver/SDL2/Engine.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Driver/SDL2/Events/Event.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Driver/SDL2/Events/Manager.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Driver/SDL2/InputDevices/GameController.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Driver/SDL2/InputDevices/Keyboard.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Driver/SDL2/InputDevices/Mouse.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Driver/SDL2/Window.hpp
)
set(TABGRAPH_SOURCE_FILES ${TABGRAPH_SOURCE_FILES}
	${CMAKE_CURRENT_SOURCE_DIR}/src/Driver/SDL2/Engine.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Driver/SDL2/Events/Event.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Driver/SDL2/Events/Manager.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Driver/SDL2/InputDevices/GameController.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Driver/SDL2/InputDevices/Keyboard.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Driver/SDL2/InputDevices/Mouse.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Driver/SDL2/Window.cpp
)
endif (MediaLibrary STREQUAL "SDL2")

if (RenderingAPI STREQUAL "OpenGL")
set(TABGRAPH_HEADER_FILES ${TABGRAPH_HEADER_FILES}
	${CMAKE_CURRENT_SOURCE_DIR}/include/Driver/OpenGL/Shader/Global.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Driver/OpenGL/Shader/Program.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Driver/OpenGL/Renderer/Context.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Driver/OpenGL/Renderer/Renderer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Driver/OpenGL/Renderer/FrameRenderer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Driver/OpenGL/Renderer/SceneRenderer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Driver/OpenGL/Renderer/Shapes/GeometryRenderer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Driver/OpenGL/Renderer/Shapes/MeshRenderer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Driver/OpenGL/Renderer/Shapes/SkyboxRenderer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Driver/OpenGL/Renderer/Light/DirectionalLightRenderer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Driver/OpenGL/Renderer/Light/HDRLightRenderer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Driver/OpenGL/Renderer/Light/SkyLightRenderer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Driver/OpenGL/Renderer/Light/PointLightRenderer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Driver/OpenGL/Texture/PixelUtils.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Driver/OpenGL/Texture/Texture.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Driver/OpenGL/Texture/Texture1D.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Driver/OpenGL/Texture/Texture1DArray.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Driver/OpenGL/Texture/Texture2D.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Driver/OpenGL/Texture/Texture2DArray.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Driver/OpenGL/Texture/Texture2DMultisample.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Driver/OpenGL/Texture/Texture2DMultisampleArray.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Driver/OpenGL/Texture/Texture3D.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Driver/OpenGL/Texture/TextureBuffer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Driver/OpenGL/Texture/TextureCubemap.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Driver/OpenGL/Texture/TextureCubemapArray.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Driver/OpenGL/Texture/TextureRectangle.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Driver/OpenGL/Texture/Sampler.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Driver/OpenGL/Texture/Framebuffer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Driver/OpenGL/Buffer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Driver/OpenGL/Compute/Object.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Driver/OpenGL/Debug.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Driver/OpenGL/ObjectHandle.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Driver/OpenGL/VertexArray.hpp
)

set(TABGRAPH_SOURCE_FILES ${TABGRAPH_SOURCE_FILES}
	${CMAKE_CURRENT_SOURCE_DIR}/src/Driver/OpenGL/Shader/Global.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Driver/OpenGL/Shader/Program.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Driver/OpenGL/Renderer/Context.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Driver/OpenGL/Renderer/Renderer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Driver/OpenGL/Renderer/FrameRenderer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Driver/OpenGL/Renderer/SceneRenderer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Driver/OpenGL/Renderer/Shapes/GeometryRenderer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Driver/OpenGL/Renderer/Shapes/MeshRenderer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Driver/OpenGL/Renderer/Shapes/SkyboxRenderer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Driver/OpenGL/Renderer/Light/DirectionalLightRenderer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Driver/OpenGL/Renderer/Light/HDRLightRenderer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Driver/OpenGL/Renderer/Light/SkyLightRenderer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Driver/OpenGL/Renderer/Light/PointLightRenderer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Driver/OpenGL/Texture/PixelUtils.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Driver/OpenGL/Texture/Texture.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Driver/OpenGL/Texture/Texture1D.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Driver/OpenGL/Texture/Texture1DArray.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Driver/OpenGL/Texture/Texture2D.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Driver/OpenGL/Texture/Texture2DArray.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Driver/OpenGL/Texture/Texture2DMultisample.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Driver/OpenGL/Texture/Texture2DMultisampleArray.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Driver/OpenGL/Texture/Texture3D.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Driver/OpenGL/Texture/TextureBuffer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Driver/OpenGL/Texture/TextureCubemap.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Driver/OpenGL/Texture/TextureCubemapArray.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Driver/OpenGL/Texture/TextureRectangle.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Driver/OpenGL/Texture/Sampler.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Driver/OpenGL/Texture/Framebuffer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Driver/OpenGL/Buffer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Driver/OpenGL/Compute/Object.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Driver/OpenGL/VertexArray.cpp
)
endif (RenderingAPI STREQUAL "OpenGL")

set(TABGRAPH_HEADER_FILES ${TABGRAPH_HEADER_FILES}
	${CMAKE_CURRENT_SOURCE_DIR}/include/Animations/Animation.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Animations/Channel.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Animations/Interpolator.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Assets/Parser.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Assets/Asset.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Assets/BinaryData.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Assets/Image.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Assets/Uri.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/brdfLUT.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Buffer/Accessor.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Buffer/Iterator.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Buffer/View.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Cameras/Camera.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Cameras/FPS.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Cameras/Orbit.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Cameras/Projection.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Common.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Compute/Object.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Config.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Inherit.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Object.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Property.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Debug.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/DLLExport.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/ECS/Common.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/ECS/ComponentArray.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/ECS/ComponentManager.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/ECS/EntityManager.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/ECS/Manager.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/ECS/System.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/ECS/SystemManager.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Engine.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Events/InputDevice/GameController.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Events/InputDevice/Keyboard.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Events/InputDevice/Mouse.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Events/DispatchQueue.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Events/Event.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Events/GameController.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Events/Keyboard.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Events/Manager.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Events/Mouse.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Events/Signal.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Light/Light.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Light/LightProbe.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Light/DirectionalLight.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Light/HDRLight.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Light/PointLight.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Light/SkyLight.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Light/SpotLight.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Material/Standard.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Material/Extension.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Material/MetallicRoughness.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Material/SpecularGlossiness.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Nodes/Node.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Nodes/Group.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Nodes/Renderable.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Nodes/Scene.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Shapes/Generators/Capsule.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Shapes/Generators/Cube.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Shapes/Generators/Plane.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Shapes/Generators/Sphere.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Shapes/Geometry.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Shapes/Mesh/Mesh.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Shapes/Mesh/MeshSkin.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Shapes/Skybox.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Shapes/Shape.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Physics/BoundingAABB.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Physics/BoundingBox.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Physics/BoundingElement.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Physics/BoundingMesh.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Physics/BoundingPlane.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Physics/BoundingSphere.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Physics/CollisionAlgorithmSAT.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Physics/CollisionAlgorithmGJKRaycast.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Physics/IntersectFunctions.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Physics/Intersection.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Physics/PhysicsEngine.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Physics/Ray.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Physics/RigidBody.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Physics/SimplexSolver.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Quadtree.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Renderer/Renderer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Renderer/Framebuffer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Renderer/FrameRenderer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Renderer/SceneRenderer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Renderer/Shapes/GeometryRenderer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Renderer/Shapes/ShapeRenderer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Renderer/Light/LightRenderer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Visitors/Visitor.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Visitors/NodeVisitor.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Visitors/SearchVisitor.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Visitors/CullVisitor.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Shader/Global.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Shader/Program.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Shader/Stage.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/SphericalHarmonics.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/StackTracer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Terrain/Terrain_GDAL.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Terrain/Terrain.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Texture/PixelUtils.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Texture/Texture.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Texture/Texture1D.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Texture/Texture1DArray.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Texture/Texture2D.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Texture/Texture2DArray.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Texture/Texture2DMultisample.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Texture/Texture2DMultisampleArray.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Texture/Texture3D.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Texture/TextureBuffer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Texture/TextureCubemap.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Texture/TextureCubemapArray.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Texture/TextureRectangle.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Texture/Sampler.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Tools/Base.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Tools/Tools.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/UI/UIManager.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Window.hpp
)

set(TABGRAPH_SOURCE_FILES ${TABGRAPH_SOURCE_FILES}
	${CMAKE_CURRENT_SOURCE_DIR}/src/Animations/Animation.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Assets/Asset.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Assets/Parser.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Assets/Image.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Assets/Uri.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Buffer/Accessor.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Buffer/View.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Cameras/Camera.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Cameras/FPS.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Cameras/Orbit.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Cameras/Projection.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Common.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Compute/Object.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Config.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/ECS/Manager.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Engine.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Events/DispatchQueue.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Events/Inputdevice/GameController.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Events/Inputdevice/Keyboard.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Events/Inputdevice/Mouse.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Events/Manager.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Light/Light.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Light/LightProbe.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Light/DirectionalLight.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Light/HDRLight.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Light/PointLight.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Light/SkyLight.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Light/SpotLight.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Material/Standard.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Material/MetallicRoughness.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Material/SpecularGlossiness.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Shapes/Generators/Capsule.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Shapes/Generators/Cube.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Shapes/Generators/Plane.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Shapes/Generators/Sphere.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Shapes/Geometry.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Shapes/Mesh/Mesh.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Shapes/Mesh/MeshSkin.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Shapes/Shape.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Shapes/Skybox.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Physics/BoundingAABB.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Physics/BoundingBox.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Physics/BoundingElement.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Physics/BoundingMesh.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Physics/BoundingPlane.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Physics/BoundingSphere.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Physics/CollisionAlgorithmSAT.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Physics/CollisionAlgorithmGJKRaycast.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Physics/Intersection.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Physics/PhysicsEngine.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Physics/RigidBody.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Physics/SimplexSolver.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Quadtree.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Renderer/Framebuffer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Renderer/FrameRenderer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Renderer/SceneRenderer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Renderer/Shapes/GeometryRenderer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Renderer/Shapes/ShapeRenderer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Renderer/Light/LightRenderer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Core/Object.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Nodes/Node.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Nodes/Renderable.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Nodes/Scene.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Visitors/CullVisitor.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Visitors/NodeVisitor.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Visitors/SearchVisitor.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Shader/Global.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Shader/Program.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Shader/Stage.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/SphericalHarmonics.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/StackTracer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Texture/PixelUtils.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Texture/Texture.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Texture/Texture1D.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Texture/Texture1DArray.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Texture/Texture2D.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Texture/Texture2DArray.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Texture/Texture2DMultisample.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Texture/Texture2DMultisampleArray.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Texture/Texture3D.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Texture/TextureBuffer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Texture/TextureCubemap.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Texture/TextureCubemapArray.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Texture/TextureRectangle.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Texture/Sampler.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Tools/Base.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Tools/Tools.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/UI/UIManager.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Window.cpp
)

set(TABGRAPH_SHADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/Lights/TransformGeometry.vert
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/Lights/DeferredDirectionnalLight.frag
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/Lights/DeferredHDRLight.frag
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/Lights/DeferredPointLight.frag
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/Lights/DeferredSkyLight.frag
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/Lights/ProbeDirectionnalLight.frag
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/Lights/ProbeHDRLight.frag
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/Lights/ProbeSkyLight.frag
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/blur.frag
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/checkOpacity.glsl
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/compositing.frag
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/deferred.frag
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/deferred.vert
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/Skybox.frag
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/forward.frag
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/forward.vert
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/forwardLit.frag
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/hzb.frag
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/metallicRoughness.frag
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/passthrough.frag
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/passthrough.vert
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/present.frag
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/Random.glsl
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/SampleShadowMap.glsl
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/specularGlossiness.frag
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/sphericalHarmonics.glsl
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/ssao.frag
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/SSR.frag
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/TemporalAccumulation.frag
)

set(PLUGINS_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/BinaryData.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/SDL2Image.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/BMP.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/BT.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/FBX/FBXDocument.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/FBX/FBXNode.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/FBX/FBXObject.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/FBX/FBXProperty.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/FBX.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/GLTF.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/HDR.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/MTLLIB.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/OBJ.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/tools.cpp
)

set(PLUGINS_HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/include/Parser/FBX/FBXDocument.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Parser/FBX/FBXNode.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Parser/FBX/FBXObject.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Parser/FBX/FBXProperty.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Parser/InternalTools.hpp
)

add_library(TabGraph STATIC ${TABGRAPH_SOURCE_FILES} ${TABGRAPH_HEADER_FILES} ${TABGRAPH_SHADER_FILES})

add_library(TabGraphPlugins OBJECT ${PLUGINS_SOURCES} ${PLUGINS_HEADERS})

target_include_directories(TabGraph PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/shaders"
	)
target_include_directories(TabGraph PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
	"${ZLIB_INCLUDE_DIR}"
	"${FASTCBASE_INCLUDES}"
	"${FASTCCORE_INCLUDES}"
	)

target_include_directories(TabGraphPlugins PRIVATE
	"${RAPIDJSON_INCLUDE_DIR}"
	)
target_link_libraries(TabGraphPlugins PRIVATE TabGraph)

if(UISubSystem EQUAL "ImGUI")
	FetchContent_Declare(
		IMGUI
		GIT_REPOSITORY	https://github.com/ocornut/imgui.git
		GIT_TAG			v1.81
	)
	FetchContent_GetProperties(IMGUI)
	if(NOT IMGUI_POPULATED)
		FetchContent_Populate(IMGUI)
		message(STATUS "Fetched IMGUI to ${imgui_SOURCE_DIR}")
		set(IMGUI_INCLUDE_DIR ${imgui_SOURCE_DIR})
		set(IMGUI_SOURCE_FILES
			${IMGUI_INCLUDE_DIR}/imgui.cpp
			${IMGUI_INCLUDE_DIR}/imgui_draw.cpp
			${IMGUI_INCLUDE_DIR}/imgui_tables.cpp
			${IMGUI_INCLUDE_DIR}/imgui_widgets.cpp
			${IMGUI_INCLUDE_DIR}/backends/imgui_impl_sdl.cpp
			${IMGUI_INCLUDE_DIR}/backends/imgui_impl_opengl3.cpp
		)
		if (RenderingAPI STREQUAL "OpenGL")
			add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLEW)
		endif (RenderingAPI STREQUAL "OpenGL")
		target_sources(TabGraph PRIVATE ${IMGUI_SOURCE_FILES})
		target_include_directories(TabGraph PRIVATE
			${IMGUI_INCLUDE_DIR}
			${IMGUI_INCLUDE_DIR}/backends
		)
	endif(NOT IMGUI_POPULATED)
endif(UISubSystem EQUAL "ImGUI")

target_link_libraries(TabGraph PUBLIC GLEW::GLEW)
target_link_libraries(TabGraph PUBLIC imagehlp)
target_link_libraries(TabGraph PUBLIC glm::glm)
target_link_libraries(TabGraph PUBLIC zlibstatic)
target_link_libraries(TabGraph PUBLIC SDL2::Core)
target_link_libraries(TabGraph PUBLIC SDL2::Image)
target_link_libraries(TabGraph PUBLIC FasTCCore)
target_link_libraries(TabGraph PUBLIC FasTCBase)

export(TARGETS TabGraph
       NAMESPACE "${CMAKE_PROJECT_NAME}-"
       APPEND FILE ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}-targets.cmake)

add_library(TabGraph::TabGraph ALIAS TabGraph)
add_library(TabGraph::Plugins ALIAS TabGraphPlugins)

if(MSVC_IDE)
	set(CMAKE_DEBUG_POSTFIX "d")
	source_group(Shaders FILES ${TABGRAPH_SHADER_FILES})
	# Macro to preserve source files hierarchy in the IDE
	macro(GroupSources curdir)
		file(GLOB children RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/${curdir} ${CMAKE_CURRENT_SOURCE_DIR}/${curdir}/*)
	
		foreach(child ${children})
			if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${curdir}/${child})
				GroupSources(${curdir}/${child})
			else()
				string(REPLACE "/" "\\" groupname ${curdir})
				string(REPLACE "src" "Sources" groupname ${groupname})
				string(REPLACE "include" "Includes" groupname ${groupname})
				#string(REPLACE "shaders" "Shaders" groupname ${groupname})
				source_group(${groupname} FILES ${CMAKE_CURRENT_SOURCE_DIR}/${curdir}/${child})
			endif()
		endforeach()
	endmacro()
	
	# Run macro
	GroupSources(src)
	GroupSources(include)
	#GroupSources(shaders)
endif()
