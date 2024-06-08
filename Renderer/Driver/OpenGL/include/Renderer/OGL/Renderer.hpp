#pragma once

#include <Renderer/OGL/GLSL/CameraUBO.hpp>
#include <Renderer/OGL/Loader/MaterialLoader.hpp>
#include <Renderer/OGL/Loader/SamplerLoader.hpp>
#include <Renderer/OGL/Loader/TextureLoader.hpp>
#include <Renderer/OGL/Loader/TextureSamplerLoader.hpp>
#include <Renderer/OGL/RAII/Wrapper.hpp>
#include <Renderer/OGL/RenderPass.hpp>
#include <Renderer/OGL/ShaderCompiler.hpp>
#include <Renderer/OGL/UniformBuffer.hpp>

#ifdef WIN32
#include <Renderer/OGL/Win32/Context.hpp>
#include <Renderer/OGL/Win32/Window.hpp>
#elif defined __linux__
#include <Renderer/OGL/Unix/Context.hpp>
#endif

#include <ECS/Registry.hpp>
#include <Renderer/Handles.hpp>
#include <Renderer/Structs.hpp>
#include <Tools/ObjectCache.hpp>

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

namespace TabGraph::Renderer {
using PrimitiveCacheKey = Tools::ObjectCacheKey<SG::Primitive*>;
using PrimitiveCache    = Tools::ObjectCache<PrimitiveCacheKey, std::shared_ptr<Primitive>>;
struct Impl {
    Impl(const CreateRendererInfo& a_Info);
    void Render();
    void Update();
    void LoadMesh(
        const ECS::DefaultRegistry::EntityRefType& a_Entity,
        const SG::Component::Mesh& a_Mesh,
        const SG::Component::Transform& a_Transform);
    std::shared_ptr<RAII::TextureSampler> LoadTextureSampler(SG::Texture* a_Texture);
    std::shared_ptr<Material> LoadMaterial(SG::Material* a_Material);

#ifdef WIN32
    Window window { "DummyWindow", "DummyWindow" };
    Context context { window.hwnd, true, {}, true, 64 };
#elif defined __linux__
    Context context { 64 };
#endif // WIN32

    uint32_t version;
    std::string name;
    PrimitiveCache primitiveCache;
    MaterialLoader materialLoader;
    ShaderCompiler shaderCompiler;
    TextureLoader textureLoader;
    SamplerLoader samplerLoader;
    TextureSamplerLoader textureSamplerLoader;

    RenderBuffer::Handle activeRenderBuffer = nullptr;
    SG::Scene* activeScene                  = nullptr;

    std::vector<RenderPassInfo> renderPasses;
    FrameBufferState forwardFrameBuffer;
    ShaderState forwardShader;
    UniformBufferT<GLSL::CameraUBO> forwardCameraUBO;
};
}
