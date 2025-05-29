include(ShaderLibrary/cmake/ShaderLibDefines.cmake)

set(MSG_RENDERER_BACKEND "OpenGL" CACHE STRING
  "MSG Renderer backend, used at compile-time")
set(MSG_RENDERER_BACKEND_VALUES
  "OpenGL;Vulkan;DirectX")
set_property(CACHE MSG_RENDERER_BACKEND PROPERTY STRINGS ${MSG_RENDERER_BACKEND_VALUES})
add_compile_definitions(
  MSG_RENDERER_OpenGL=0
  MSG_RENDERER_Vulkan=1
  MSG_RENDERER_DirectX=2
  MSG_RENDERER_BACKEND=MSG_RENDERER_${MSG_RENDERER_BACKEND_VALUES})

set(MSG_EVENTS_BACKEND "SDL2" CACHE STRING
  "MSG Events backend, used at compile-time")
set(MSG_EVENTS_BACKEND_VALUES
  "SDL2")
set_property(CACHE MSG_EVENTS_BACKEND PROPERTY STRINGS ${MSG_EVENTS_BACKEND_VALUES})
add_compile_definitions(
  MSG_EVENTS_SDL2=0
  MSG_EVENTS_BACKEND=MSG_EVENTS_${MSG_EVENTS_BACKEND_VALUES})

add_subdirectory(ShaderLibrary)
add_subdirectory(Debug)
add_subdirectory(SparseSet)
add_subdirectory(MemoryPool)
add_subdirectory(ECS)
add_subdirectory(Threads)
add_subdirectory(PageFile)
add_subdirectory(Parenting)
add_subdirectory(Shapes)
add_subdirectory(BoundingVolume)
add_subdirectory(Tools)
add_subdirectory(SH)
add_subdirectory(Events)
add_subdirectory(Core)
add_subdirectory(Transform)
add_subdirectory(Camera)
add_subdirectory(Buffer)
add_subdirectory(PixelDescriptor)
add_subdirectory(Image)
add_subdirectory(Texture)
add_subdirectory(BRDF)
add_subdirectory(Material)
add_subdirectory(Mesh)
add_subdirectory(Light)
add_subdirectory(Entity)
add_subdirectory(Animation)
add_subdirectory(Scene)
add_subdirectory(ShapeGenerator)
add_subdirectory(PrimitiveOptimizer)
add_subdirectory(OGL)
add_subdirectory(Renderer)
add_subdirectory(SwapChain)
add_subdirectory(Window)
add_subdirectory(Keyboard)
add_subdirectory(Mouse)
add_subdirectory(Assets)
add_subdirectory(FogArea)

GenerateShaderLib()
