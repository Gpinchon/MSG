#pragma once

#include <Renderer/OGL/CPULightCuller.hpp>
#include <Renderer/OGL/Context.hpp>
#include <Renderer/OGL/GPULightCuller.hpp>
#include <Renderer/OGL/Loader/MaterialLoader.hpp>
#include <Renderer/OGL/Loader/SamplerLoader.hpp>
#include <Renderer/OGL/Loader/TextureLoader.hpp>
#include <Renderer/OGL/RAII/Wrapper.hpp>
#include <Renderer/OGL/RendererPath.hpp>
#include <Renderer/OGL/ShaderCompiler.hpp>
#include <Renderer/OGL/UniformBufferUpdate.hpp>

#include <ECS/Registry.hpp>
#include <Renderer/Handles.hpp>
#include <Renderer/Structs.hpp>
#include <Tools/FixedSizeMemoryPool.hpp>
#include <Tools/ObjectCache.hpp>

#include <Camera.glsl>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>

namespace MSG {
class Material;
class Mesh;
class MeshSkin;
class MeshPrimitive;
class Scene;
class Texture;
}

namespace MSG::Core {
class Transform;
}

namespace MSG::Renderer {
class Primitive;
class Material;
struct CreateRendererInfo;
}

namespace MSG::Renderer::RAII {
class VertexArray;
}

namespace MSG::Renderer {
using PrimitiveCacheKey = Tools::ObjectCacheKey<MeshPrimitive*>;
using PrimitiveCache    = Tools::ObjectCache<PrimitiveCacheKey, std::shared_ptr<Primitive>>;
class Impl {
public:
    Impl(const CreateRendererInfo& a_Info, const RendererSettings& a_Settings);
    void Render();
    void Update();
    void UpdateMeshes();
    void UpdateTransforms();
    void UpdateSkins();
    void UpdateCamera();
    void LoadMesh(
        const ECS::DefaultRegistry::EntityRefType& a_Entity,
        const Mesh& a_Mesh,
        const Core::Transform& a_Transform);
    void LoadMeshSkin(
        const ECS::DefaultRegistry::EntityRefType& a_Entity,
        const MeshSkin& a_MeshSkin);
    void SetSettings(const RendererSettings& a_Settings);
    void SetActiveRenderBuffer(const RenderBuffer::Handle& a_RenderBuffer);
    std::shared_ptr<RAII::Texture> LoadTexture(MSG::Texture* a_Texture);
    std::shared_ptr<RAII::Sampler> LoadSampler(MSG::Sampler* a_Sampler);
    std::shared_ptr<Material> LoadMaterial(MSG::Material* a_Material);

    Context context;
    bool enableTAA      = true;
    uint64_t frameIndex = 0;
    uint32_t version;
    std::string name;
    PrimitiveCache primitiveCache;
    MaterialLoader materialLoader;
    ShaderCompiler shaderCompiler;
    TextureLoader textureLoader;
    SamplerLoader samplerLoader;

    RenderBuffer::Handle activeRenderBuffer = nullptr;
    Scene* activeScene                      = nullptr;

    std::shared_ptr<Path> path;

    std::vector<UniformBufferUpdate> uboToUpdate; // the UBOs that will be updated on each Update call
    UniformBufferT<GLSL::CameraUBO> cameraUBO;
    std::shared_ptr<RAII::Sampler> IblSpecSampler;
    std::shared_ptr<RAII::Sampler> BrdfLutSampler;
    std::shared_ptr<RAII::Texture> BrdfLut;

    GPULightCuller lightCuller { *this };
};
}
