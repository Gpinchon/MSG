#pragma once

#include <MSG/ECS/Registry.hpp>
#include <MSG/FixedSizeMemoryPool.hpp>
#include <MSG/ModulesLibrary.hpp>
#include <MSG/OGLCmdBuffer.hpp>
#include <MSG/OGLContext.hpp>
#include <MSG/OGLFence.hpp>
#include <MSG/Renderer/Handles.hpp>
#include <MSG/Renderer/OGL/Loader/MaterialLoader.hpp>
#include <MSG/Renderer/OGL/Loader/SamplerLoader.hpp>
#include <MSG/Renderer/OGL/Loader/SparseTextureLoader.hpp>
#include <MSG/Renderer/OGL/Loader/TextureLoader.hpp>
#include <MSG/Renderer/OGL/ObjectRepertory.hpp>
#include <MSG/Renderer/OGL/ShaderCompiler.hpp>
#include <MSG/Renderer/OGL/TextureBlurHelper.hpp>
#include <MSG/Renderer/RenderPassInterface.hpp>
#include <MSG/Renderer/Structs.hpp>
#include <MSG/Renderer/SubsystemInterface.hpp>
#include <MSG/Tools/ObjectCache.hpp>

#include <Camera.glsl>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>

namespace Msg {
class Material;
class Mesh;
class MeshSkin;
class MeshPrimitive;
class Scene;
class Texture;
class Transform;
class OGLVertexArray;
}

namespace Msg::Renderer {
class Primitive;
class Material;
struct CreateRendererInfo;
}

namespace Msg::Renderer {
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
    std::shared_ptr<OGLTexture> LoadTexture(Msg::Texture* a_Texture, const bool& a_Sparse = false);
    std::shared_ptr<OGLSampler> LoadSampler(Msg::Sampler* a_Sampler);
    std::shared_ptr<Material> LoadMaterial(Msg::Material* a_Material);

    OGLContext context;
    OGLCmdBuffer renderCmdBuffer;
    OGLFence renderFence;

    RendererSettings settings;
    uint64_t frameIndex = 0;
    uint32_t version;
    std::string name;

    ObjectRepertory<std::shared_ptr<OGLProgram>> shaderCache;
    PrimitiveCache primitiveCache;
    MaterialLoader materialLoader;
    ShaderCompiler shaderCompiler;
    TextureLoader textureLoader;
    SparseTextureLoader sparseTextureLoader;
    SamplerLoader samplerLoader;

    ModulesLibrary<SubsystemInterface> subsystemsLibrary;
    ModulesLibrary<RenderPassInterface> renderPassesLibrary;

    RenderBuffer::Handle activeRenderBuffer = nullptr;
    Scene* activeScene                      = nullptr;

    // useful tool for fullscreen draws
    TextureBlurHelpers blurHelpers;
    std::shared_ptr<OGLVertexArray> presentVAO;
};
}
