#pragma once

#include <MSG/ECS/Registry.hpp>
#include <MSG/FixedSizeMemoryPool.hpp>
#include <MSG/OGLContext.hpp>
#include <MSG/OGLFence.hpp>
#include <MSG/Renderer/Handles.hpp>
#include <MSG/Renderer/OGL/Loader/MaterialLoader.hpp>
#include <MSG/Renderer/OGL/Loader/SamplerLoader.hpp>
#include <MSG/Renderer/OGL/Loader/TextureLoader.hpp>
#include <MSG/Renderer/OGL/ObjectRepertory.hpp>
#include <MSG/Renderer/OGL/RendererPath.hpp>
#include <MSG/Renderer/OGL/ShaderCompiler.hpp>
#include <MSG/Renderer/OGL/Subsystems/SubsystemLibrary.hpp>
#include <MSG/Renderer/Structs.hpp>
#include <MSG/Tools/ObjectCache.hpp>

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
class Transform;
}

namespace MSG::Renderer {
class Primitive;
class Material;
struct CreateRendererInfo;
}

namespace MSG::Renderer {
using PrimitiveCacheKey = Tools::ObjectCacheKey<MeshPrimitive*>;
using PrimitiveCache    = Tools::ObjectCache<PrimitiveCacheKey, std::shared_ptr<Primitive>>;
class Impl {
public:
    Impl(const CreateRendererInfo& a_Info, const RendererSettings& a_Settings);
    void Render();
    void Update();
    void LoadMesh(
        const ECS::DefaultRegistry::EntityRefType& a_Entity,
        const Mesh& a_Mesh,
        const Transform& a_Transform);
    void LoadMeshSkin(
        const ECS::DefaultRegistry::EntityRefType& a_Entity,
        const MeshSkin& a_MeshSkin);
    void SetSettings(const RendererSettings& a_Settings);
    void SetActiveRenderBuffer(const RenderBuffer::Handle& a_RenderBuffer);
    std::shared_ptr<OGLTexture> LoadTexture(MSG::Texture* a_Texture);
    std::shared_ptr<OGLSampler> LoadSampler(MSG::Sampler* a_Sampler);
    std::shared_ptr<Material> LoadMaterial(MSG::Material* a_Material);

    OGLContext context;
    QualitySetting ssaoQuality;
    QualitySetting shadowQuality;
    bool enableTAA      = true;
    uint64_t frameIndex = 0;
    uint32_t version;
    std::string name;

    ObjectRepertory<std::shared_ptr<OGLProgram>> shaderCache;
    PrimitiveCache primitiveCache;
    MaterialLoader materialLoader;
    ShaderCompiler shaderCompiler;
    TextureLoader textureLoader;
    SamplerLoader samplerLoader;

    RenderBuffer::Handle activeRenderBuffer = nullptr;
    Scene* activeScene                      = nullptr;

    std::shared_ptr<Path> path;
    SubsystemLibrary subsystemsLibrary;
};
}
