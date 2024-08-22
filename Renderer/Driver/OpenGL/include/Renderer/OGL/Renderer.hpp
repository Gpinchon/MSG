#pragma once

#include <Renderer/OGL/CPULightCuller.hpp>
#include <Renderer/OGL/GPULightCuller.hpp>
#include <Renderer/OGL/Loader/MaterialLoader.hpp>
#include <Renderer/OGL/Loader/SamplerLoader.hpp>
#include <Renderer/OGL/Loader/TextureLoader.hpp>
#include <Renderer/OGL/RAII/Wrapper.hpp>
#include <Renderer/OGL/RendererPath.hpp>
#include <Renderer/OGL/ShaderCompiler.hpp>
#include <Renderer/OGL/UniformBufferUpdate.hpp>

#ifdef WIN32
#include <Renderer/OGL/Win32/Context.hpp>
#include <Renderer/OGL/Win32/Window.hpp>
#elif defined __linux__
#include <Renderer/OGL/Unix/Context.hpp>
#endif

#include <ECS/Registry.hpp>
#include <Renderer/Handles.hpp>
#include <Renderer/Structs.hpp>
#include <Tools/FixedSizeMemoryPool.hpp>
#include <Tools/ObjectCache.hpp>

#include <Camera.glsl>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>

namespace TabGraph::SG::Component {
class Mesh;
class Transform;
}

namespace TabGraph::SG {
class Material;
class Primitive;
class Scene;
class Texture;
}

namespace TabGraph::Renderer {
class Primitive;
class Material;
struct CreateRendererInfo;
}

namespace TabGraph::Renderer::RAII {
class VertexArray;
}
/**
 * @todo
 * implement Deferred rendering
 * add light volumes
 * add temporal accumulation
 * add screen space reflection
 * add support for shadowing
 * add support for CSM
 * add sparse virtual textures https://studiopixl.com/2022-04-27/sparse-virtual-textures
 */

namespace TabGraph::Renderer {
using PrimitiveCacheKey = Tools::ObjectCacheKey<SG::Primitive*>;
using PrimitiveCache    = Tools::ObjectCache<PrimitiveCacheKey, std::shared_ptr<Primitive>>;
class Impl {
public:
    Impl(const CreateRendererInfo& a_Info, const RendererSettings& a_Settings);
    void Render();
    void Update();
    void UpdateMeshes();
    void UpdateTransforms();
    void UpdateCamera();
    void LoadMesh(
        const ECS::DefaultRegistry::EntityRefType& a_Entity,
        const SG::Component::Mesh& a_Mesh,
        const SG::Component::Transform& a_Transform);
    void SetSettings(const RendererSettings& a_Settings);
    void SetActiveRenderBuffer(const RenderBuffer::Handle& a_RenderBuffer);
    std::shared_ptr<RAII::Texture> LoadTexture(SG::Texture* a_Texture);
    std::shared_ptr<RAII::Sampler> LoadSampler(SG::Sampler* a_Sampler);
    std::shared_ptr<Material> LoadMaterial(SG::Material* a_Material);

#ifdef WIN32
    RAII::Window window { "DummyWindow", "DummyWindow" };
    Context context { window.hwnd, true, {}, true, 64 };
#elif defined __linux__
    Context context;
#endif // WIN32
    uint64_t frameIndex = 0;
    uint32_t version;
    std::string name;
    PrimitiveCache primitiveCache;
    MaterialLoader materialLoader;
    ShaderCompiler shaderCompiler;
    TextureLoader textureLoader;
    SamplerLoader samplerLoader;

    RenderBuffer::Handle activeRenderBuffer = nullptr;
    SG::Scene* activeScene                  = nullptr;

    std::shared_ptr<Path> path;

    std::vector<UniformBufferUpdate> uboToUpdate; // the UBOs that will be updated on each Update call
    UniformBufferT<GLSL::CameraUBO> cameraUBO;
    std::shared_ptr<RAII::Sampler> IblSpecSampler;
    std::shared_ptr<RAII::Sampler> BrdfLutSampler;
    std::shared_ptr<RAII::Texture> BrdfLut;

    GPULightCuller lightCuller { *this };
};
}
