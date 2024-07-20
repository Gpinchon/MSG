#pragma once

#include <Renderer/OGL/CPULightCuller.hpp>
#include <Renderer/OGL/GPULightCuller.hpp>
#include <Renderer/OGL/Loader/MaterialLoader.hpp>
#include <Renderer/OGL/Loader/SamplerLoader.hpp>
#include <Renderer/OGL/Loader/TextureLoader.hpp>
#include <Renderer/OGL/RAII/Wrapper.hpp>
#include <Renderer/OGL/RenderPass.hpp>
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
#include <optional>
#include <string>
#include <unordered_map>

namespace TabGraph::SG::Component {
struct Mesh;
struct Transform;
}

namespace TabGraph::SG {
struct Material;
struct Primitive;
struct Scene;
struct Texture;
}

namespace TabGraph::Renderer {
struct Primitive;
struct Material;
struct CreateRendererInfo;
}

namespace TabGraph::Renderer::RAII {
struct VertexArray;
}

// TODO implement Deferred rendering
// TODO add light volumes
// TODO add weighted blended order-independent transparency transparency https://jcgt.org/published/0002/02/09/
// TODO add temporal accumulation
// TODO add screen space reflection
// TODO add support for shadowing
// TODO add support for CSM
// TODO add sparse virtual textures https://studiopixl.com/2022-04-27/sparse-virtual-textures

namespace TabGraph::Renderer {
using PrimitiveCacheKey = Tools::ObjectCacheKey<SG::Primitive*>;
using PrimitiveCache    = Tools::ObjectCache<PrimitiveCacheKey, std::shared_ptr<Primitive>>;
struct Impl {
    Impl(const CreateRendererInfo& a_Info);
    void Render();
    void Update();
    void UpdateCamera();
    void UpdateForwardPass();
    void UpdatePresentPass();
    void LoadMesh(
        const ECS::DefaultRegistry::EntityRefType& a_Entity,
        const SG::Component::Mesh& a_Mesh,
        const SG::Component::Transform& a_Transform);
    void SetActiveRenderBuffer(const RenderBuffer::Handle& a_RenderBuffer);
    std::shared_ptr<RAII::Texture> LoadTexture(SG::Image* a_Image);
    std::shared_ptr<RAII::Sampler> LoadSampler(SG::TextureSampler* a_Sampler);
    std::shared_ptr<Material> LoadMaterial(SG::Material* a_Material);
    std::shared_ptr<RenderPass> CreateRenderPass(const RenderPassInfo& a_Info);

#ifdef WIN32
    Window window { "DummyWindow", "DummyWindow" };
    Context context { window.hwnd, true, {}, true, 64 };
#elif defined __linux__
    Context context;
#endif // WIN32

    uint32_t version;
    std::string name;
    PrimitiveCache primitiveCache;
    MaterialLoader materialLoader;
    ShaderCompiler shaderCompiler;
    TextureLoader textureLoader;
    SamplerLoader samplerLoader;

    RenderBuffer::Handle activeRenderBuffer = nullptr;
    SG::Scene* activeScene                  = nullptr;

    std::vector<UniformBufferUpdate> uboToUpdate; // the UBOs that will be updated on each Update call

    Tools::FixedSizeMemoryPool<RenderPass, 32> renderPassMemoryPool;
    std::shared_ptr<RAII::Sampler> LUTSampler;
    std::shared_ptr<RAII::Texture> GGX_LUT;
    std::shared_ptr<RAII::Texture> BRDF_LUT;

    ShaderState fwdMetRoughShader;
    ShaderState fwdSpecGlossShader;
    UniformBufferT<GLSL::Camera> fwdCameraUBO;
    std::shared_ptr<RAII::FrameBuffer> fwdFB;
    std::shared_ptr<RenderPass> fwdRenderPass;

    std::shared_ptr<RenderPass> presentRenderPass;

    GPULightCuller lightCuller { *this };
};
}
